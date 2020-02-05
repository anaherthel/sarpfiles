\ENCODING=ISO-8859-1
\Problem name: bSARP

Maximize
 obj: - 3 x(0,3,0) - 3 x(0,4,0) - 3 x(0,5,0) + 2 x(4,7,0) + 2 x(5,7,0)
Subject To
 Constraint1_0:   x(0,3,0) + x(0,4,0) + x(0,5,0) + x(0,7,0)  = 1
 Constraint1_1:   x(3,7,0) + x(4,7,0) + x(5,7,0)  = 1
 Constraint2_0:    = 1
 Constraint2_1:   x(0,3,0) + x(0,4,0) + x(0,5,0)  = 1
 Constraint3_0:    = 1
 Constraint4_0:   x(0,7,0) + x(3,7,0) + x(4,7,0) + x(5,7,0)  = 1
 Constraint5_2:   x(4,7,0) <= 1
 Constraint5_3:   x(5,7,0) <= 1
 Constraint6_0_0: x(0,3,0) + x(0,4,0) + x(0,5,0) + x(0,7,0)  = 0
 Constraint6_1_0:  = 0
 Constraint6_2_0:  = 0
 Constraint6_3_0: - x(0,3,0) + x(3,7,0)  = 0
 Constraint6_4_0: - x(0,4,0) + x(4,7,0)  = 0
 Constraint6_5_0: - x(0,5,0) + x(5,7,0)  = 0
Bounds
 0 <= x(0,3,0) <= 1
 0 <= x(0,4,0) <= 1
 0 <= x(0,5,0) <= 1
 0 <= x(0,7,0) <= 1
 0 <= x(3,7,0) <= 1
 0 <= x(4,7,0) <= 1
 0 <= x(5,7,0) <= 1
Binaries
 x(0,3,0)  x(0,4,0)  x(0,5,0)  x(0,7,0)  x(3,7,0)  x(4,7,0)  x(5,7,0) 
End
