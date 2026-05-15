double normalize_angle(double a)
{
    while (a > 180.0) a -= 360.0;
    while (a < -180.0) a += 360.0;
    return a;
}

double angle_lerp(double a, double b, double t)
{
    double diff = normalize_angle(b - a);
    return a + diff * t;
}