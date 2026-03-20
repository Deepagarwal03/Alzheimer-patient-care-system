#pragma once
#include <cstdarg>
namespace Eloquent {
    namespace ML {
        namespace Port {
            class MotionClassifier {
                public:
                    /**
                    * Predict class for features vector
                    */
                    int predict(float *x) {
                        uint8_t votes[3] = { 0 };
                        // tree #1
                        if (x[6] <= 876.484992980957) {
                            votes[0] += 1;
                        }

                        else {
                            if (x[6] <= 5163.840087890625) {
                                votes[1] += 1;
                            }

                            else {
                                votes[2] += 1;
                            }
                        }

                        // tree #2
                        if (x[6] <= 880.3349914550781) {
                            votes[0] += 1;
                        }

                        else {
                            if (x[6] <= 5068.155029296875) {
                                votes[1] += 1;
                            }

                            else {
                                votes[2] += 1;
                            }
                        }

                        // tree #3
                        if (x[5] <= 11671.9150390625) {
                            if (x[6] <= 5375.9599609375) {
                                votes[1] += 1;
                            }

                            else {
                                votes[2] += 1;
                            }
                        }

                        else {
                            if (x[5] <= 17208.189453125) {
                                if (x[5] <= 16883.28515625) {
                                    if (x[5] <= 12791.97021484375) {
                                        if (x[6] <= 5910.2249755859375) {
                                            votes[1] += 1;
                                        }

                                        else {
                                            votes[2] += 1;
                                        }
                                    }

                                    else {
                                        if (x[6] <= 6612.47509765625) {
                                            votes[1] += 1;
                                        }

                                        else {
                                            votes[2] += 1;
                                        }
                                    }
                                }

                                else {
                                    if (x[6] <= 1124.8449783325195) {
                                        votes[0] += 1;
                                    }

                                    else {
                                        if (x[5] <= 16922.9951171875) {
                                            votes[2] += 1;
                                        }

                                        else {
                                            votes[1] += 1;
                                        }
                                    }
                                }
                            }

                            else {
                                if (x[6] <= 6012.065185546875) {
                                    if (x[5] <= 17266.859375) {
                                        if (x[6] <= 927.9449844360352) {
                                            votes[0] += 1;
                                        }

                                        else {
                                            votes[1] += 1;
                                        }
                                    }

                                    else {
                                        votes[1] += 1;
                                    }
                                }

                                else {
                                    votes[2] += 1;
                                }
                            }
                        }

                        // tree #4
                        if (x[6] <= 876.484992980957) {
                            votes[0] += 1;
                        }

                        else {
                            if (x[5] <= 10677.2099609375) {
                                votes[2] += 1;
                            }

                            else {
                                if (x[5] <= 22622.0947265625) {
                                    if (x[6] <= 5642.675048828125) {
                                        votes[1] += 1;
                                    }

                                    else {
                                        votes[2] += 1;
                                    }
                                }

                                else {
                                    if (x[5] <= 23802.3505859375) {
                                        if (x[5] <= 23626.7646484375) {
                                            votes[2] += 1;
                                        }

                                        else {
                                            votes[1] += 1;
                                        }
                                    }

                                    else {
                                        if (x[6] <= 6035.580078125) {
                                            votes[1] += 1;
                                        }

                                        else {
                                            votes[2] += 1;
                                        }
                                    }
                                }
                            }
                        }

                        // tree #5
                        if (x[6] <= 5805.635009765625) {
                            if (x[5] <= 17276.8095703125) {
                                if (x[5] <= 16883.28515625) {
                                    if (x[5] <= 16863.1357421875) {
                                        if (x[6] <= 799.8699913024902) {
                                            votes[0] += 1;
                                        }

                                        else {
                                            votes[1] += 1;
                                        }
                                    }

                                    else {
                                        if (x[5] <= 16882.35546875) {
                                            votes[0] += 1;
                                        }

                                        else {
                                            votes[1] += 1;
                                        }
                                    }
                                }

                                else {
                                    if (x[5] <= 17222.0) {
                                        if (x[6] <= 1128.6949768066406) {
                                            votes[0] += 1;
                                        }

                                        else {
                                            votes[1] += 1;
                                        }
                                    }

                                    else {
                                        if (x[5] <= 17255.5146484375) {
                                            votes[1] += 1;
                                        }

                                        else {
                                            votes[0] += 1;
                                        }
                                    }
                                }
                            }

                            else {
                                if (x[6] <= 880.4549751281738) {
                                    votes[0] += 1;
                                }

                                else {
                                    votes[1] += 1;
                                }
                            }
                        }

                        else {
                            votes[2] += 1;
                        }

                        // tree #6
                        if (x[6] <= 880.3349914550781) {
                            votes[0] += 1;
                        }

                        else {
                            if (x[5] <= 10724.794921875) {
                                if (x[5] <= 9254.8798828125) {
                                    votes[2] += 1;
                                }

                                else {
                                    if (x[5] <= 9441.85498046875) {
                                        votes[1] += 1;
                                    }

                                    else {
                                        if (x[6] <= 5962.764892578125) {
                                            votes[1] += 1;
                                        }

                                        else {
                                            votes[2] += 1;
                                        }
                                    }
                                }
                            }

                            else {
                                if (x[5] <= 22622.0947265625) {
                                    if (x[6] <= 6032.130126953125) {
                                        votes[1] += 1;
                                    }

                                    else {
                                        votes[2] += 1;
                                    }
                                }

                                else {
                                    if (x[5] <= 28095.73046875) {
                                        if (x[6] <= 4997.85009765625) {
                                            votes[1] += 1;
                                        }

                                        else {
                                            votes[2] += 1;
                                        }
                                    }

                                    else {
                                        votes[2] += 1;
                                    }
                                }
                            }
                        }

                        // tree #7
                        if (x[5] <= 11172.66015625) {
                            if (x[6] <= 5566.885009765625) {
                                votes[1] += 1;
                            }

                            else {
                                votes[2] += 1;
                            }
                        }

                        else {
                            if (x[5] <= 17268.564453125) {
                                if (x[5] <= 16883.28515625) {
                                    if (x[6] <= 5068.14501953125) {
                                        if (x[5] <= 16814.44921875) {
                                            votes[1] += 1;
                                        }

                                        else {
                                            votes[1] += 1;
                                        }
                                    }

                                    else {
                                        votes[2] += 1;
                                    }
                                }

                                else {
                                    if (x[5] <= 17160.6953125) {
                                        if (x[5] <= 16965.0849609375) {
                                            votes[0] += 1;
                                        }

                                        else {
                                            votes[0] += 1;
                                        }
                                    }

                                    else {
                                        if (x[5] <= 17252.8544921875) {
                                            votes[1] += 1;
                                        }

                                        else {
                                            votes[0] += 1;
                                        }
                                    }
                                }
                            }

                            else {
                                if (x[5] <= 22961.78515625) {
                                    if (x[6] <= 6025.4150390625) {
                                        if (x[6] <= 880.4549751281738) {
                                            votes[0] += 1;
                                        }

                                        else {
                                            votes[1] += 1;
                                        }
                                    }

                                    else {
                                        votes[2] += 1;
                                    }
                                }

                                else {
                                    if (x[6] <= 4997.85009765625) {
                                        votes[1] += 1;
                                    }

                                    else {
                                        votes[2] += 1;
                                    }
                                }
                            }
                        }

                        // tree #8
                        if (x[5] <= 10677.2099609375) {
                            if (x[5] <= 9254.8798828125) {
                                votes[2] += 1;
                            }

                            else {
                                if (x[6] <= 5806.454833984375) {
                                    votes[1] += 1;
                                }

                                else {
                                    votes[2] += 1;
                                }
                            }
                        }

                        else {
                            if (x[5] <= 17240.6748046875) {
                                if (x[6] <= 873.9699935913086) {
                                    votes[0] += 1;
                                }

                                else {
                                    if (x[5] <= 13994.375) {
                                        if (x[6] <= 5524.18994140625) {
                                            votes[1] += 1;
                                        }

                                        else {
                                            votes[2] += 1;
                                        }
                                    }

                                    else {
                                        if (x[6] <= 5068.14501953125) {
                                            votes[1] += 1;
                                        }

                                        else {
                                            votes[2] += 1;
                                        }
                                    }
                                }
                            }

                            else {
                                if (x[5] <= 21996.8955078125) {
                                    if (x[5] <= 19527.8505859375) {
                                        if (x[6] <= 6703.47509765625) {
                                            votes[1] += 1;
                                        }

                                        else {
                                            votes[2] += 1;
                                        }
                                    }

                                    else {
                                        if (x[6] <= 6125.43017578125) {
                                            votes[1] += 1;
                                        }

                                        else {
                                            votes[2] += 1;
                                        }
                                    }
                                }

                                else {
                                    if (x[6] <= 5163.030029296875) {
                                        votes[1] += 1;
                                    }

                                    else {
                                        votes[2] += 1;
                                    }
                                }
                            }
                        }

                        // tree #9
                        if (x[5] <= 11172.66015625) {
                            if (x[6] <= 6234.0751953125) {
                                votes[1] += 1;
                            }

                            else {
                                votes[2] += 1;
                            }
                        }

                        else {
                            if (x[6] <= 885.9599914550781) {
                                votes[0] += 1;
                            }

                            else {
                                if (x[6] <= 5642.675048828125) {
                                    votes[1] += 1;
                                }

                                else {
                                    votes[2] += 1;
                                }
                            }
                        }

                        // tree #10
                        if (x[5] <= 10677.2099609375) {
                            if (x[6] <= 5114.679931640625) {
                                votes[1] += 1;
                            }

                            else {
                                votes[2] += 1;
                            }
                        }

                        else {
                            if (x[5] <= 17240.1904296875) {
                                if (x[5] <= 16883.28515625) {
                                    if (x[5] <= 12791.97021484375) {
                                        if (x[5] <= 10831.25537109375) {
                                            votes[1] += 1;
                                        }

                                        else {
                                            votes[1] += 1;
                                        }
                                    }

                                    else {
                                        if (x[6] <= 4949.669921875) {
                                            votes[1] += 1;
                                        }

                                        else {
                                            votes[2] += 1;
                                        }
                                    }
                                }

                                else {
                                    if (x[5] <= 17132.01953125) {
                                        if (x[5] <= 16908.8046875) {
                                            votes[0] += 1;
                                        }

                                        else {
                                            votes[0] += 1;
                                        }
                                    }

                                    else {
                                        if (x[6] <= 999.9599838256836) {
                                            votes[0] += 1;
                                        }

                                        else {
                                            votes[1] += 1;
                                        }
                                    }
                                }
                            }

                            else {
                                if (x[6] <= 5163.85009765625) {
                                    votes[1] += 1;
                                }

                                else {
                                    votes[2] += 1;
                                }
                            }
                        }

                        // return argmax of votes
                        uint8_t classIdx = 0;
                        float maxVotes = votes[0];

                        for (uint8_t i = 1; i < 3; i++) {
                            if (votes[i] > maxVotes) {
                                classIdx = i;
                                maxVotes = votes[i];
                            }
                        }

                        return classIdx;
                    }

                protected:
                };
            }
        }
    }