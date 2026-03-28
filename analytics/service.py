from __future__ import annotations
import argparse,csv,json,pickle,threading,time
from collections import deque
from datetime import datetime,timedelta
from pathlib import Path
from urllib.error import URLError
from urllib.request import urlopen
import numpy as np
from flask import Flask,jsonify
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score
from sklearn.model_selection import train_test_split

STATE=["Stable Rest","Sleep Recovery","Active Mobility","Environmental Stress","Agitated","Fall Emergency"]
FEED=["Comfortable","Encourage Hydration","Adjust Room Warmer","Cool Room Down","Calm Patient","Emergency Assist"]
RAW=["timestamp_iso","timestamp_ms","motion","motion_code","env_status","env_code","body_temp_c","env_temp_c","humidity_pct","heart_bpm","distance_cm","rfid_primary","rfid_secondary","pir_motion","pir_flag","near_motion","heart_ir"]
OUT=["created_at_iso","patient_state","patient_state_confidence","comfort_feedback","feedback_confidence","alert_level","recommended_action","risk_factors","state_summary","feedback_summary","cnn_window_size"]

sf=lambda v,d=0.0: d if v in (None,"") else float(v)
def si(v,d=0):
    try:return int(float(v))
    except:return d

def mcode(t):
    s=str(t or "").lower()
    return 2 if "fall" in s else 1 if ("walk" in s or "moving" in s) else 0

def ecode(t):
    s=str(t or "").lower()
    return 0 if "cold" in s else 2 if "hot" in s else 1

def hourf(iso):
    try:d=datetime.fromisoformat(str(iso))
    except:return .5
    return (d.hour*60+d.minute)/1440

def norm(r):
    iso=str(r.get("timestamp_iso") or datetime.now().isoformat(timespec="seconds"))
    motion=str(r.get("motion") or "Stationary")
    env=str(r.get("env_status") or "Ideal State")
    et=sf(r.get("env_temp_c"),24.0); bt=sf(r.get("body_temp_c"),et)
    return {"timestamp_iso":iso,"timestamp_ms":si(r.get("timestamp_ms"),0),"motion":motion,"motion_code":si(r.get("motion_code"),mcode(motion)),"env_status":env,"env_code":si(r.get("env_code"),ecode(env)),"body_temp_c":bt,"env_temp_c":et,"humidity_pct":sf(r.get("humidity_pct"),52.0),"heart_bpm":sf(r.get("heart_bpm"),74.0),"distance_cm":sf(r.get("distance_cm"),90.0),"rfid_primary":str(r.get("rfid_primary") or "Unknown"),"rfid_secondary":str(r.get("rfid_secondary") or "None"),"pir_motion":str(r.get("pir_motion") or "No Motion"),"pir_flag":si(r.get("pir_flag"),1 if "motion" in str(r.get("pir_motion","")).lower() else 0),"near_motion":str(r.get("near_motion") or "Stationary"),"heart_ir":sf(r.get("heart_ir"),0.0)}

def label(r):
    r=norm(r); h=r["heart_bpm"]; et=r["env_temp_c"]; bt=r["body_temp_c"]; hum=r["humidity_pct"]; mc=r["motion_code"]; pf=r["pir_flag"]; hf=hourf(r["timestamp_iso"])
    if mc==2 or h>=128 or bt>=39.0:return STATE[5],FEED[5]
    if h>=102 and mc==0:return STATE[4],FEED[4]
    if et<20:return STATE[3],FEED[2]
    if et>29 or hum>=74:return STATE[3],FEED[3]
    if mc==1 or (pf==1 and h>=82):return STATE[2],FEED[1]
    if (hf>=.82 or hf<=.22) and mc==0 and h<=70:return STATE[1],FEED[0]
    return STATE[0],FEED[0]

def vec(r):
    r=norm(r)
    return np.asarray([r["heart_bpm"],r["body_temp_c"],r["env_temp_c"],r["humidity_pct"],r["distance_cm"],r["motion_code"],r["pir_flag"],min(r["heart_ir"]/100000.0,2.0),hourf(r["timestamp_iso"]),r["env_code"]],dtype=np.float32)

class CNN:
    def __init__(self,w,f,c,filters=24,k=3,lr=.015,epochs=70,bs=32,seed=42):
        self.w,self.f,self.c,self.filters,self.k,self.lr,self.epochs,self.bs,self.seed=w,f,c,filters,k,lr,epochs,bs,seed
    def init(self):
        g=np.random.default_rng(self.seed); s=.14
        self.cw=g.normal(0,s,(self.filters,self.k,self.f)).astype(np.float32); self.cb=np.zeros(self.filters,np.float32)
        self.dw=g.normal(0,s,(self.filters,self.c)).astype(np.float32); self.db=np.zeros(self.c,np.float32)
    def soft(self,z): z=z-z.max(1,keepdims=True); e=np.exp(z); return e/e.sum(1,keepdims=True)
    def conv(self,x):
        n,t,_=x.shape; out=np.zeros((n,t-self.k+1,self.filters),np.float32)
        for i in range(t-self.k+1): out[:,i,:]=np.tensordot(x[:,i:i+self.k,:],self.cw,axes=([1,2],[1,2]))+self.cb
        return out
    def fit(self,x,y):
        self.init(); g=np.random.default_rng(self.seed)
        for _ in range(self.epochs):
            for s in range(0,len(x),self.bs):
                idx=g.permutation(len(x))[s:s+self.bs]; xb=x[idx]; yb=y[idx]; n=max(1,len(xb))
                c=self.conv(xb); r=np.maximum(c,0); p=r.mean(1); z=p@self.dw+self.db; pr=self.soft(z)
                dz=pr; dz[np.arange(n),yb]-=1; dz/=n; ddw=p.T@dz; ddb=dz.sum(0); dp=dz@self.dw.T; steps=r.shape[1]
                dc=np.repeat((dp/max(1,steps))[:,None,:],steps,1)*(c>0); dcw=np.zeros_like(self.cw)
                for i in range(steps): dcw+=np.tensordot(dc[:,i,:],xb[:,i:i+self.k,:],axes=([0],[0]))
                self.dw-=self.lr*ddw.astype(np.float32); self.db-=self.lr*ddb.astype(np.float32); self.cw-=self.lr*dcw.astype(np.float32); self.cb-=self.lr*dc.sum((0,1)).astype(np.float32)
    def predict_proba(self,x):
        c=self.conv(x); r=np.maximum(c,0); p=r.mean(1); return self.soft(p@self.dw+self.db)
    def predict(self,x): return np.argmax(self.predict_proba(x),1)

def synth(kind,ts,g):
    if kind==0: heart,et,hum,motion,pf,dist,bt=g.normal(72,5),g.normal(24.5,1.2),g.normal(51,6),"Stationary",0,g.normal(105,25),g.normal(36.7,.2)
    elif kind==1: heart,et,hum,motion,pf,dist,bt=g.normal(63,4),g.normal(23.2,1.0),g.normal(50,5),"Stationary",0,g.normal(118,18),g.normal(36.4,.2)
    elif kind==2: heart,et,hum,motion,pf,dist,bt=g.normal(94,8),g.normal(24.8,1.5),g.normal(55,7),"Walking",1,g.normal(130,35),g.normal(37.1,.25)
    elif kind==3: heart,et,hum,motion,pf,dist,bt=g.normal(84,7),g.normal(17.5,1.1),g.normal(46,5),"Stationary",0,g.normal(100,22),g.normal(36.3,.2)
    elif kind==4: heart,et,hum,motion,pf,dist,bt=g.normal(101,8),g.normal(31.0,1.3),g.normal(76,5),"Stationary",0,g.normal(95,20),g.normal(37.6,.28)
    elif kind==5: heart,et,hum,motion,pf,dist,bt=g.normal(112,8),g.normal(24.0,1.6),g.normal(57,8),"Stationary",1,g.normal(92,20),g.normal(37.3,.3)
    else: heart,et,hum,motion,pf,dist,bt=g.normal(129,9),g.normal(24.2,1.4),g.normal(58,7),"!!! FALL !!!",1,g.normal(24,8),g.normal(37.8,.35)
    env="Cold State" if et<20 else "Hot State" if et>29 else "Ideal State"
    r=norm({"timestamp_iso":ts.isoformat(timespec="seconds"),"timestamp_ms":int(ts.timestamp()*1000),"motion":motion,"env_status":env,"body_temp_c":float(bt),"env_temp_c":float(et),"humidity_pct":float(hum),"heart_bpm":float(heart),"distance_cm":max(5.0,float(dist)),"rfid_primary":"SAMPLE-TAG","rfid_secondary":"None","pir_motion":"Motion Detected" if pf else "No Motion","pir_flag":pf,"near_motion":"Moving" if pf else "Stationary","heart_ir":float(g.normal(90000 if pf else 70000,8000))})
    s,f=label(r); r["patient_state"],r["comfort_feedback"]=s,f; return r
def ensure_csv(path,fields):
    path.parent.mkdir(parents=True,exist_ok=True)
    if not path.exists():
        with path.open("w",newline="",encoding="utf-8") as h: csv.DictWriter(h,fieldnames=fields).writeheader()

def seed_file(path,rows=1600):
    ensure_csv(path,RAW+["patient_state","comfort_feedback"]); g=np.random.default_rng(42); t=datetime.now().replace(minute=0,second=0,microsecond=0)-timedelta(minutes=rows)
    with path.open("w",newline="",encoding="utf-8") as h:
        w=csv.DictWriter(h,fieldnames=RAW+["patient_state","comfort_feedback"]); w.writeheader()
        for i in range(rows): w.writerow(synth(i%7,t+timedelta(minutes=i),g))

def rows(path):
    if not path.exists(): return []
    with path.open("r",newline="",encoding="utf-8") as h: return [dict(r) for r in csv.DictReader(h)]

def train(seed_path,raw_path,window=12):
    recs=rows(seed_path)+[dict(norm(r),patient_state=label(r)[0],comfort_feedback=label(r)[1]) for r in rows(raw_path)]
    recs.sort(key=lambda x:x["timestamp_iso"])
    x=np.asarray([vec(r) for r in recs],np.float32); fy=np.asarray([FEED.index(r["comfort_feedback"]) for r in recs],np.int32)
    rx1,rx2,ry1,ry2=train_test_split(x,fy,test_size=.2,random_state=42,stratify=fy)
    rf=RandomForestClassifier(n_estimators=260,max_depth=12,min_samples_leaf=2,random_state=42,class_weight="balanced_subsample"); rf.fit(rx1,ry1); rfa=float(accuracy_score(ry2,rf.predict(rx2)))
    seq=[]; sy=[]
    for i in range(window-1,len(recs)): seq.append(np.stack([vec(r) for r in recs[i-window+1:i+1]],0)); sy.append(STATE.index(recs[i]["patient_state"]))
    sx=np.asarray(seq,np.float32); sy=np.asarray(sy,np.int32); mean=sx.mean((0,1),keepdims=True); std=sx.std((0,1),keepdims=True); std[std<1e-6]=1; sx=(sx-mean)/std
    cx1,cx2,cy1,cy2=train_test_split(sx,sy,test_size=.2,random_state=42,stratify=sy)
    cnn=CNN(window,sx.shape[2],len(STATE)); cnn.fit(cx1,cy1); cnna=float(accuracy_score(cy2,cnn.predict(cx2)))
    return {"rf":rf,"cnn":cnn,"mean":mean.squeeze((0,1)),"std":std.squeeze((0,1)),"window":window,"metrics":{"cnn_accuracy":cnna,"rf_accuracy":rfa,"training_rows":len(recs)}}

def infer(hist,art):
    rs=[norm(r) for r in hist]; rs=([rs[0]]*(art["window"]-len(rs))+rs if len(rs)<art["window"] else rs[-art["window"]:])
    sx=np.stack([vec(r) for r in rs],0); sx=(sx-art["mean"])/art["std"]; sp=art["cnn"].predict_proba(sx[None,:,:])[0]; si=int(np.argmax(sp)); state=STATE[si]
    last=rs[-1]; fp=art["rf"].predict_proba(vec(last)[None,:])[0]; fc=[FEED[int(i)] for i in art["rf"].classes_]; fi=int(np.argmax(fp)); feed=fc[fi]
    alert="Urgent" if state==STATE[5] or feed==FEED[5] else "Observe" if state in {STATE[3],STATE[4]} else "Stable"
    risks=[]
    if last["motion_code"]==2: risks.append("fall pattern detected")
    if last["heart_bpm"]>=105: risks.append("elevated heart rate")
    if last["env_temp_c"]<20: risks.append("room temperature too low")
    if last["env_temp_c"]>29: risks.append("room temperature too high")
    if last["humidity_pct"]>=74: risks.append("humidity above comfort range")
    if last["distance_cm"]<=35: risks.append("patient very close to nearby obstacle or floor")
    if last["pir_flag"]==1 and last["motion_code"]==0 and last["heart_bpm"]>=100: risks.append("stationary stress spike")
    if not risks: risks=["no active high-risk trigger"]
    act={FEED[0]:"Patient comfort is stable. Keep current room conditions and continue routine monitoring.",FEED[1]:"Patient is active. Offer water, supervise movement, and check fatigue.",FEED[2]:"Room appears cold. Increase warmth, add a blanket, and reassess.",FEED[3]:"Room appears hot or humid. Improve cooling and check hydration.",FEED[4]:"Signs of agitation are present. Reduce noise, reassure the patient, and observe closely.",FEED[5]:"Emergency pattern detected. Check the patient now and follow your fall-response procedure."}
    return {"created_at_iso":datetime.now().isoformat(timespec="seconds"),"patient_state":state,"patient_state_confidence":float(sp[si]),"comfort_feedback":feed,"feedback_confidence":float(fp[fi]),"alert_level":alert,"recommended_action":act[feed],"risk_factors":risks,"state_summary":f"The CNN sees the recent {art['window']}-sample trend as {state.lower()} with {(sp[si]*100):.1f}% confidence.","feedback_summary":"The Random Forest recommends the current comfort action from the latest vital and environment pattern.","cnn_window_size":art["window"]}

class Service:
    def __init__(self,url,poll=2.0,window=12):
        self.url=url.rstrip("/"); self.poll=poll; self.window=window; self.base=Path(__file__).resolve().parent; self.data=self.base/"data"; self.seed=self.data/"synthetic_seed_dataset.csv"; self.raw=self.data/"raw_sensor_log.csv"; self.proc=self.data/"processed_predictions.csv"
        self.lock=threading.Lock(); self.stop=threading.Event(); self.hist=deque(maxlen=window*2); self.phist=deque(maxlen=12); self.latest_raw=None; self.latest_proc=None; self.last_poll=None; self.last_train=None; self.msg="Waiting for the first ESP32 sample."; ensure_csv(self.raw,RAW); ensure_csv(self.proc,OUT)
        if not self.seed.exists(): seed_file(self.seed)
        self.raw_rows=max(0,len(rows(self.raw))); self.proc_rows=max(0,len(rows(self.proc))); [self.hist.append(norm(r)) for r in rows(self.raw)[-self.hist.maxlen:]]; self.art=train(self.seed,self.raw,window); self.last_train=datetime.now().isoformat(timespec="seconds"); self.train_mark=self.raw_rows; self.msg=f"Models ready. CNN accuracy {self.art['metrics']['cnn_accuracy']*100:.1f}% and Random Forest accuracy {self.art['metrics']['rf_accuracy']*100:.1f}%."
    def fetch(self):
        with urlopen(f"{self.url}/data",timeout=4) as r: p=json.loads(r.read().decode("utf-8"))
        pir=str(p.get("pir") or p.get("near") or p.get("nearMotion") or "No Motion"); et=p.get("envTemp",p.get("temp")); bt=p.get("bodyTemp",p.get("temp"))
        return norm({"timestamp_iso":datetime.now().isoformat(timespec="seconds"),"timestamp_ms":int(time.time()*1000),"motion":p.get("motion"),"env_status":p.get("env") or p.get("envStatus"),"body_temp_c":bt,"env_temp_c":et,"humidity_pct":p.get("hum") or p.get("humidity"),"heart_bpm":p.get("heart") or p.get("heartRate"),"distance_cm":p.get("dist") or p.get("distance"),"rfid_primary":p.get("rfid") or p.get("rfid1"),"rfid_secondary":p.get("rfid2") or "None","pir_motion":pir,"pir_flag":1 if "motion" in pir.lower() else 0,"near_motion":p.get("nearMotion") or p.get("near") or pir,"heart_ir":p.get("ir") or p.get("heartIr")})
    def step(self):
        raw=self.fetch()
        with self.lock:
            with self.raw.open("a",newline="",encoding="utf-8") as h: csv.DictWriter(h,fieldnames=RAW).writerow(raw)
            self.raw_rows+=1; self.last_poll=raw["timestamp_iso"]; self.latest_raw=raw; self.hist.append(raw)
            if self.raw_rows-self.train_mark>=60: self.art=train(self.seed,self.raw,self.window); self.last_train=datetime.now().isoformat(timespec="seconds"); self.train_mark=self.raw_rows
            out=infer(list(self.hist),self.art); store=dict(out); store["risk_factors"]=" | ".join(out["risk_factors"])
            with self.proc.open("a",newline="",encoding="utf-8") as h: csv.DictWriter(h,fieldnames=OUT).writerow(store)
            self.proc_rows+=1; self.latest_proc=out; self.phist.append(out); self.msg=f"Saved raw sample {self.raw_rows} and processed prediction {self.proc_rows}. Latest alert level: {out['alert_level']}."
    def loop(self):
        while not self.stop.is_set():
            start=time.time()
            try:self.step()
            except URLError as e: self.msg=f"Could not reach ESP32 at {self.url}. Check the ESP32 access point and your browser machine network. ({e.reason})"
            except Exception as e: self.msg=f"Analytics polling error: {e}"
            self.stop.wait(max(.25,self.poll-(time.time()-start)))
    def start(self): threading.Thread(target=self.loop,daemon=True,name="sensor-analytics").start()
    def snap(self):
        with self.lock:
            return {"service":{"status":"online","esp32_url":self.url,"message":self.msg,"raw_rows":self.raw_rows,"processed_rows":self.proc_rows,"last_poll_iso":self.last_poll,"last_train_iso":self.last_train,"models_ready":True,"window_size":self.window,"raw_log_path":str(self.raw),"processed_log_path":str(self.proc),"metrics":self.art["metrics"]},"raw":self.latest_raw,"processed":self.latest_proc,"history":[{"created_at_iso":x.get("created_at_iso"),"patient_state":x.get("patient_state"),"alert_level":x.get("alert_level"),"comfort_feedback":x.get("comfort_feedback")} for x in list(self.phist)]}

def app(service):
    a=Flask(__name__)
    @a.after_request
    def cors(r): r.headers["Access-Control-Allow-Origin"]="*"; r.headers["Access-Control-Allow-Headers"]="Content-Type"; r.headers["Access-Control-Allow-Methods"]="GET,POST,OPTIONS"; return r
    @a.route("/")
    def root(): return jsonify({"service":"sensor-analytics","hint":"Use /api/latest for dashboard predictions."})
    @a.route("/api/latest")
    def latest(): return jsonify(service.snap())
    @a.route("/api/health")
    def health(): s=service.snap()["service"]; return jsonify({"status":"ok","message":s["message"],"models_ready":s["models_ready"]})
    return a

def main():
    p=argparse.ArgumentParser(description="Local analytics/logger service for the ESP32 patient monitor.")
    p.add_argument("--esp32-url",default="http://192.168.4.1")
    p.add_argument("--host",default="127.0.0.1")
    p.add_argument("--port",type=int,default=5001)
    p.add_argument("--poll-interval",type=float,default=2.0)
    a=p.parse_args(); s=Service(a.esp32_url,a.poll_interval); s.start(); app(s).run(host=a.host,port=a.port,debug=False,threaded=True)

if __name__=="__main__": main()

