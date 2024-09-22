Coordinates = eval(input())
# [(12.5,11.3), (2.0,12.0), (18.7,16.2), (12.5,7.0)]
# Left points
if Coordinates[0][0] <= Coordinates[1][0]:
    left_min1 = Coordinates[0]
    min1_index = 0
    left_min2 = Coordinates[1]
    min2_index = 1
else:
    left_min2 = Coordinates[0]
    min2_index = 0
    left_min1 = Coordinates[1]
    min1_index = 1
if Coordinates[2][0] <= left_min2[0]:
    if Coordinates[2][0] <= left_min1[0]:
        left_min2 = left_min1
        min2_index = 2
        left_min1 = Coordinates[2]
        min1_index = 2
    else:
        left_min2 = Coordinates[2]
        min2_index = 2
if Coordinates[3][0] <= left_min2[0]:
    if Coordinates[3][0] <= left_min1[0]:
        left_min2 = left_min1
        min2_index = min1_index
        left_min1 = Coordinates[3]
        min1_index = 3
    else:
        left_min2 = Coordinates[3]
        min2_index = 3
left_point = left_min1
left_point_index = min1_index
if left_min2[0] == left_min1[0]:
    if abs(left_min1[1]) < abs(left_min2[1]):
        left_point = left_min1
        left_point_index = min1_index
    else:
        left_point = left_min2
        left_point_index = min2_index
# Minimum leftmost point and its index are found

# Right points
if Coordinates[0][0] >= Coordinates[1][0]:
    right_max1 = Coordinates[0]
    right_max2 = Coordinates[1]
else:
    right_max2 = Coordinates[0]
    right_max1 = Coordinates[1]
if Coordinates[2][0] >= right_max2[0]:
    if Coordinates[2][0] >= right_max1[0]:
        right_max2 = right_max1
        right_max1 = Coordinates[2]
    else:
        right_max2 = Coordinates[2]
if Coordinates[3][0] >= right_max2[0]:
    if Coordinates[3][0] >= right_max1[0]:
        right_max2 = right_max1
        right_max1 = Coordinates[3]
    else:
        right_max2 = Coordinates[3]
right_point = right_max1
if right_max1[0] == right_max2[0]:
    if abs(right_max1[1]) < abs(right_max2[1]):
        right_point = right_max1
    else:
        right_point = right_max2
# Minimum rightmost point is found

# Line equation = (x-left_point[0])* (right_point[1] - left_point[1]) = y - right_point[1]
# Groping the points
Upper_points = []
Lower_points = []
if Coordinates[0] != left_point and Coordinates[0] != right_point:
    if ((abs(Coordinates[0][1]) - abs(left_point[1])) * (right_point[0] - left_point[0]) > (abs(right_point[1]) - abs(left_point[1])) * (Coordinates[0][0] - left_point[0])):
        Upper_points.append(Coordinates[0])
    else:
        Lower_points.append(Coordinates[0])
if Coordinates[1] != left_point and Coordinates[1] != right_point:
    if ((abs(Coordinates[1][1]) - abs(left_point[1])) * (right_point[0] - left_point[0]) > (abs(right_point[1]) - abs(left_point[1])) * (Coordinates[1][0] - left_point[0])):
        Upper_points.append(Coordinates[1])
    else:
        Lower_points.append(Coordinates[1])
if Coordinates[2] != left_point and Coordinates[2] != right_point:
    if ((abs(Coordinates[2][1]) - abs(left_point[1])) * (right_point[0] - left_point[0]) > (abs(right_point[1]) - abs(left_point[1])) * (Coordinates[2][0] - left_point[0])):
        Upper_points.append(Coordinates[2])
    else:
        Lower_points.append(Coordinates[2])
if Coordinates[3] != left_point and Coordinates[3] != right_point:
    if ((abs(Coordinates[3][1]) - abs(left_point[1])) * (right_point[0] - left_point[0]) > (abs(right_point[1]) - abs(left_point[1])) * (Coordinates[3][0] - left_point[0])):
        Upper_points.append(Coordinates[3])
    else:
        Lower_points.append(Coordinates[3])
# Upper and lower points are found

# Area under the line that contains min leftmost and rightmost points.
Green_Area = ((abs(left_point[1]) + abs(right_point[1])) * (right_point[0] - left_point[0])) / 2
a = left_point_index + 1
if a == 4:  # if left_point_index is 3 then we need to take the first coordinate in the Coordinates list.
    a = 0
if len(Upper_points) == 2:
    if (Coordinates[a] in Upper_points and Coordinates[left_point_index - 1] in Upper_points):
        if Coordinates[a][1] >= 0 and Coordinates[left_point_index - 1][1] >= 0:  # if the triangle is in below of the coordinate system
            b = left_point_index - 1
        else:
            b = a
        Included_Triangle = abs((left_point[0] * (Coordinates[b][1] - right_point[1]) + Coordinates[b][0] * (right_point[1] - left_point[1]) + right_point[0] * (left_point[1] - Coordinates[b][1])) / 2.0)
        Green_Area += Included_Triangle
elif len(Upper_points) == 1 and len(Lower_points) == 1:
    Triangle = abs((Lower_points[0][0] * (left_point[1] - right_point[1]) + left_point[0] * (right_point[1] - Lower_points[0][1]) + right_point[0] * (Lower_points[0][1] - left_point[1])) / 2.0)
    Green_Area -= Triangle
else:
    First_Triangle = abs((Coordinates[0][0] * (Coordinates[1][1] - Coordinates[2][1]) + Coordinates[1][0] * (Coordinates[2][1] - Coordinates[0][1]) + Coordinates[2][0] * (Coordinates[0][1] - Coordinates[1][1])) / 2.0)
    Second_Triangle = abs((Coordinates[2][0] * (Coordinates[3][1] - Coordinates[0][1]) + Coordinates[3][0] * (Coordinates[0][1] - Coordinates[2][1]) + Coordinates[0][0] * (Coordinates[2][1] - Coordinates[3][1])) / 2.0)
    First_C = Coordinates[0:3]
    Point4 = Coordinates[3]
    Areas1 = abs((First_C[0][0] * (First_C[1][1] - Point4[1]) + First_C[1][0] * (Point4[1] - First_C[0][1]) + Point4[0] * (First_C[0][1] - First_C[1][1])) / 2.0) \
             + abs((First_C[2][0] * (First_C[1][1] - Point4[1]) + First_C[1][0] * (Point4[1] - First_C[2][1]) + Point4[0] * (First_C[2][1] - First_C[1][1])) / 2.0) \
             + abs((First_C[2][0] * (First_C[0][1] - Point4[1]) + First_C[0][0] * (Point4[1] - First_C[2][1]) + Point4[0] * (First_C[2][1] -First_C[0][1])) / 2.0)
    Point2 = Coordinates[1]
    Second_C = [Coordinates[0], Coordinates[2], Coordinates[3]]
    Areas2 = abs((Second_C[0][0] * (Second_C[1][1] - Point2[1]) + Second_C[1][0] * (Point2[1] - Second_C[0][1]) + Point2[0] * (Second_C[0][1] - Second_C[1][1])) / 2.0) \
             + abs((Second_C[2][0] * (Second_C[1][1] - Point2[1]) + Second_C[1][0] * (Point2[1] - Second_C[2][1]) + Point2[0] * (Second_C[2][1] - Second_C[1][1])) / 2.0) \
             + abs((Second_C[2][0] * (Second_C[0][1] -Point2[1]) +Second_C[0][0] * (Point2[1] -Second_C[2][1]) +Point2[0] * (Second_C[2][1] -Second_C[0][1])) / 2.0)
    if abs(Areas1 - First_Triangle < 0.0000001) or abs(Areas2 - Second_Triangle < 0.0000001):
        Quad_Area = abs(First_Triangle - Second_Triangle)
    else:
        Quad_Area = First_Triangle + Second_Triangle
    if (Coordinates[a] not in Upper_points and Coordinates[a] not in Lower_points) or (Coordinates[left_point_index - 1] not in Lower_points and Coordinates[left_point_index - 1] not in Lower_points):
        Green_Area -= Quad_Area  # If leftmost and rightmost points are successive.
    else:
        if left_point[1] <= 0 and right_point[1] <= 0 and Coordinates[left_point_index-1][1] <= 0:
            b = left_point_index - 1
        else:
            b = a
        Reduntant_Triangle = abs((left_point[0] * (Coordinates[b][1] - right_point[1]) + Coordinates[b][0] * (right_point[1] - left_point[1]) + right_point[0] * (left_point[1] - Coordinates[b][1])) / 2.0)
        Green_Area -= (Quad_Area + Reduntant_Triangle)
Green_Area = f"{Green_Area:.2f}"
print(Green_Area)

