\ENCODING=ISO-8859-1
\Problem name: nSARP

Maximize
 obj1: 15.0406 x(12,12,0) + 8.2234 x(12,13,0) - 6.8172 x(13,12,0)
       - 6.9736 x(14,12,0) + 0 y(14) + 0 y(15)
Subject To
 Constraint1_0:      = 0
 Constraint1_1:      = 0
 Constraint1_2:      = 0
 Constraint1_3:      = 0
 Constraint1_4:      = 0
 Constraint1_5:      = 0
 Constraint1_6:      = 0
 Constraint1_7:      = 0
 Constraint1_8:      = 0
 Constraint1_9:      = 0
 Constraint1_10:     = 0
 Constraint1_11:     = 0
 Constraint2_12:    x(12,12,0) + x(12,13,0) - y(12)  = 0
 Constraint3_12_0:  x(12,12,0) + x(12,13,0) - x(13,12,0) - x(13,13,0)
                    - x(13,15,0)  = 0
 Constraint4_0_0:    = 0
 Constraint4_1_0:    = 0
 Constraint4_2_0:    = 0
 Constraint4_3_0:    = 0
 Constraint4_4_0:    = 0
 Constraint4_5_0:    = 0
 Constraint4_6_0:    = 0
 Constraint4_7_0:    = 0
 Constraint4_8_0:    = 0
 Constraint4_9_0:    = 0
 Constraint4_10_0:   = 0
 Constraint4_11_0:   = 0
 Constraint4_12_0:  x(12,13,0) - x(13,12,0) - x(14,12,0)  = 0
 Constraint4_13_0:  - x(12,13,0) + x(13,12,0) + x(13,15,0)  = 0
 Constraint5_0:     x(14,12,0) + x(14,15,0)  = 1
 Constraint6_0:     x(13,15,0) + x(14,15,0)  = 1
 Constraint7_0:     - 48 y(0) + b(0) <= 0
 Constraint7_1:     - 48 y(1) + b(1) <= 0
 Constraint7_2:     - 48 y(2) + b(2) <= 0
 Constraint7_3:     - 48 y(3) + b(3) <= 0
 Constraint7_4:     - 48 y(4) + b(4) <= 0
 Constraint7_5:     - 48 y(5) + b(5) <= 0
 Constraint7_6:     - 48 y(6) + b(6) <= 0
 Constraint7_7:     - 48 y(7) + b(7) <= 0
 Constraint7_8:     - 48 y(8) + b(8) <= 0
 Constraint7_9:     - 48 y(9) + b(9) <= 0
 Constraint7_10:    - 48 y(10) + b(10) <= 0
 Constraint7_11:    - 48 y(11) + b(11) <= 0
 Constraint7_12:    - 48 y(12) + b(12) <= 0
 Constraint7_13:    - 48 y(13) + b(13) <= 0
 Constraint8_12:    b(12) - b(13) <= 0
 Constraint9_12_12: 48 x(12,12,0) <= 47.9166666666667
 Constraint9_12_13: 48 x(12,13,0) + b(12) - b(13) <= 47.5552032520325
 Constraint9_13_12: 48 x(13,12,0) - b(12) + b(13) <= 47.5552032520325
 Constraint9_13_13: 48 x(13,13,0) <= 47.9166666666667
 Constraint9_13_15: 48 x(13,15,0) + b(13) - b(15) <= 47.9166666666667
 Constraint9_14_12: 48 x(14,12,0) - b(12) + b(14) <= 47.630243902439
 Constraint9_14_15: 48 x(14,15,0) + b(14) - b(15) <= 48
 Constraint11_0:    b(0) <= 10.0166666666667
 Constraint12_0:    b(0) >= 10.0166666666667
 Constraint11_1:    b(1) <= 5.35
 Constraint12_1:    b(1) >= 5.35
 Constraint11_2:    b(2) <= 23.1833333333333
 Constraint12_2:    b(2) >= 23.1833333333333
 Constraint11_3:    b(3) <= 22.9271327913279
 Constraint12_3:    b(3) >= 22.9271327913279
 Constraint11_4:    b(4) <= 15.0166666666667
 Constraint12_4:    b(4) >= 15.0166666666667
 Constraint11_5:    b(5) <= 23.0166666666667
 Constraint12_5:    b(5) >= 23.0166666666667
 Constraint11_6:    b(6) <= 8.75
 Constraint12_6:    b(6) >= 8.75
 Constraint11_7:    b(7) <= 9.16666666666667
 Constraint12_7:    b(7) >= 9.16666666666667
 Constraint11_8:    b(8) <= 20.7833333333333
 Constraint12_8:    b(8) >= 20.7833333333333
 Constraint11_9:    b(9) <= 18.9666666666667
 Constraint12_9:    b(9) >= 18.9666666666667
 Constraint11_10:   b(10) <= 6.75
 Constraint12_10:   b(10) >= 6.75
 Constraint11_11:   b(11) <= 21.1666666666667
 Constraint12_11:   b(11) >= 21.1666666666667
 Constraint11_12:   b(12) <= 24
 Constraint12_12:   b(12) >= 0
 Constraint11_13:   b(13) <= 24
 Constraint12_13:   b(13) >= 0
 Constraint11_14:   b(14) <= 24
 Constraint12_14:   b(14) >= 0
 Constraint11_15:   b(15) <= 24
 Constraint12_15:   b(15) >= 0
 Constraint13_14:   - b(14) + b(15) <= 8
Bounds
 0 <= x(12,12,0) <= 1
 0 <= x(12,13,0) <= 1
 0 <= x(13,12,0) <= 1
 0 <= x(13,13,0) <= 1
 0 <= x(13,15,0) <= 1
 0 <= x(14,12,0) <= 1
 0 <= x(14,15,0) <= 1
 0 <= y(0) <= 1
 0 <= y(1) <= 1
 0 <= y(2) <= 1
 0 <= y(3) <= 1
 0 <= y(4) <= 1
 0 <= y(5) <= 1
 0 <= y(6) <= 1
 0 <= y(7) <= 1
 0 <= y(8) <= 1
 0 <= y(9) <= 1
 0 <= y(10) <= 1
 0 <= y(11) <= 1
 0 <= y(12) <= 1
 0 <= y(13) <= 1
 0 <= y(14) <= 1
 0 <= y(15) <= 1
 0 <= b(0) <= 24
 0 <= b(1) <= 24
 0 <= b(2) <= 24
 0 <= b(3) <= 24
 0 <= b(4) <= 24
 0 <= b(5) <= 24
 0 <= b(6) <= 24
 0 <= b(7) <= 24
 0 <= b(8) <= 24
 0 <= b(9) <= 24
 0 <= b(10) <= 24
 0 <= b(11) <= 24
 0 <= b(12) <= 24
 0 <= b(13) <= 24
 0 <= b(14) <= 24
 0 <= b(15) <= 24
Binaries
 x(12,12,0)  x(12,13,0)  x(13,12,0)  x(13,13,0)  x(13,15,0)  x(14,12,0) 
 x(14,15,0)  y(0)  y(1)  y(2)  y(3)  y(4)  y(5)  y(6)  y(7)  y(8)  y(9)  y(10) 
 y(11)  y(12)  y(13)  y(14)  y(15) 
End
