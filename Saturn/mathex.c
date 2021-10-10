#include <jo/jo.h>
#include "mathex.h"

void ex_Translate_point(jo_pos3D_fixed * toTranslate, const jo_vector_fixed * delta)
{
    toTranslate->x += delta->x;
    toTranslate->y += delta->y;
    toTranslate->z += delta->z;
}

void ex_Vector_normal(jo_vector_fixed * a)
{
    jo_fixed length = jo_fixed_sqrt(jo_fixed_mult(a->x, a->x) + jo_fixed_mult(a->y, a->y) + jo_fixed_mult(a->z, a->z));
    
    // Do not divide if length is zero
    if (length != 0)
    {
        a->x = jo_fixed_div(a->x, length);
        a->y = jo_fixed_div(a->y, length);
        a->z = jo_fixed_div(a->z, length);
    }
}

jo_vector_fixed ex_Vector_cross(const jo_vector_fixed * a, const jo_vector_fixed * b)
{
    jo_vector_fixed cross;
    cross.x = jo_fixed_mult(a->y, b->z) - jo_fixed_mult(a->z, b->y);
    cross.y = jo_fixed_mult(a->z, b->x) - jo_fixed_mult(a->x, b->z);
    cross.z = jo_fixed_mult(a->x, b->y) - jo_fixed_mult(a->y, b->x);
    return cross;
}

jo_fixed ex_Dot_product(const jo_fixed * a, const jo_fixed * b)
{
    jo_fixed result = 0;

    for (int i = 0; i < XYZ; i++)
    {
        result += jo_fixed_mult(a[i], b[i]);
    }

    return result;
}

void ex_Get_matrix_from_coordinate_system(const ex_CoordinateSystem * coordSys, FIXED (*result)[XYZ])
{
    // Get normalized cross of direction and up vector
    jo_vector_fixed y = ex_Vector_cross(&coordSys->Dir, &coordSys->Up);
    ex_Vector_normal(&y);

    // SGL matrix is in 4x3 format
    result[0][0] = coordSys->Dir.x;
    result[1][0] = coordSys->Dir.y;
    result[2][0] = coordSys->Dir.z;
    result[3][0] = coordSys->Origin.x;
    result[0][1] = y.x;
    result[1][1] = y.y;
    result[2][1] = y.z;
    result[3][1] = coordSys->Origin.y;
    result[0][2] = coordSys->Up.x;
    result[1][2] = coordSys->Up.y;
    result[2][2] = coordSys->Up.z;
    result[3][2] = coordSys->Origin.z;
}

jo_pos3D_fixed ex_Project_point_to_axis(const jo_pos3D_fixed * toProject, const jo_pos3D_fixed * origin, const jo_vector_fixed * direction)
{
    // Get vector from origin to point
    jo_fixed originPoint[XYZ];
    originPoint[X] = toProject->x - origin->x;
    originPoint[Y] = toProject->y - origin->y;
    originPoint[Z] = toProject->z - origin->z;

    // The jo_fixed_dot takes array of XYZ fixed values instead of jo_vector_fixed
    jo_fixed dotDirection[XYZ];
    dotDirection[X] = direction->x;
    dotDirection[Y] = direction->y;
    dotDirection[Z] = direction->z;

    // Get dot product of axis direction and origin->point vector
    jo_fixed dot = ex_Dot_product(originPoint, dotDirection);

    // Get projected point, by multipling dot product with direction vector and offseting by origin
    jo_pos3D_fixed projected;
    projected.x = jo_fixed_mult(direction->x, dot) + origin->x;
    projected.y = jo_fixed_mult(direction->y, dot) + origin->y;
    projected.z = jo_fixed_mult(direction->z, dot) + origin->z;

    return projected;
}

jo_vector_fixed ex_Rotate_vector_around_axis(const jo_vector_fixed * toRotate, const jo_fixed rad, const jo_vector_fixed * direction)
{
    // Get radius of the rotation circle
    jo_fixed radius = jo_fixed_sqrt(jo_fixed_mult(toRotate->x, toRotate->x) + jo_fixed_mult(toRotate->y, toRotate->y) + jo_fixed_mult(toRotate->z, toRotate->z));

    if (radius != 0)
    {
        // Normal vector must be unit vector
        jo_vector_fixed axisNormal;
        axisNormal.x = jo_fixed_div(toRotate->x, radius);
        axisNormal.y = jo_fixed_div(toRotate->y, radius);
        axisNormal.z = jo_fixed_div(toRotate->z, radius);

        // Get cross vector between normal and direction of axis
        jo_vector_fixed cross = ex_Vector_cross(&axisNormal, direction);
        ex_Vector_normal(&cross);

        // Get cos and sin
        jo_fixed cos = jo_fixed_cos(rad);
        jo_fixed sin = jo_fixed_sin(rad);

        // Get rotated normal
        jo_vector_fixed rotatedNormal;
        rotatedNormal.x = jo_fixed_mult(axisNormal.x, cos) + jo_fixed_mult(cross.x, sin);
        rotatedNormal.y = jo_fixed_mult(axisNormal.y, cos) + jo_fixed_mult(cross.y, sin);
        rotatedNormal.z = jo_fixed_mult(axisNormal.z, cos) + jo_fixed_mult(cross.z, sin);

        // Rotated normal must be normalized before it can be used, to prevent drifting
        ex_Vector_normal(&rotatedNormal);

        // Resize rotated vector to the same size as it was before rotated
        jo_vector_fixed result;
        result.x = jo_fixed_mult(rotatedNormal.x, radius);
        result.y = jo_fixed_mult(rotatedNormal.y, radius);
        result.z = jo_fixed_mult(rotatedNormal.z, radius);
        return result;
    }
    else
    {
        // If radius (vector length) is 0, point cannot be rotated
        jo_vector_fixed result;
        result.x = toRotate->x;
        result.y = toRotate->y;
        result.z = toRotate->z;
        return result;
    }
}

jo_pos3D_fixed ex_Rotate_point_around_axis(const jo_pos3D_fixed * toRotate, const jo_fixed rad, const jo_pos3D_fixed * origin, const jo_vector_fixed * direction)
{
    // Get normal vector of the axis
    jo_pos3D_fixed projected = ex_Project_point_to_axis(toRotate, origin, direction);
    jo_vector_fixed axisNormal;
    axisNormal.x = projected.x - toRotate->x;
    axisNormal.y = projected.y - toRotate->y;
    axisNormal.z = projected.z - toRotate->z;

    // Get radius of the rotation circle
    jo_fixed radius = jo_fixed_sqrt(jo_fixed_mult(axisNormal.x, axisNormal.x) + jo_fixed_mult(axisNormal.y, axisNormal.y) + jo_fixed_mult(axisNormal.z, axisNormal.z));

    if (radius != 0)
    {
        // Normal vector must be unit vector
        axisNormal.x = jo_fixed_div(axisNormal.x, radius);
        axisNormal.y = jo_fixed_div(axisNormal.y, radius);
        axisNormal.z = jo_fixed_div(axisNormal.z, radius);

        // Get cross vector between normal and direction of axis
        jo_vector_fixed cross = ex_Vector_cross(&axisNormal, direction);
        ex_Vector_normal(&cross);

        // Get cos and sin
        jo_fixed cos = jo_fixed_cos(rad);
        jo_fixed sin = jo_fixed_sin(rad);

        // Get rotated normal
        jo_vector_fixed rotatedNormal;
        rotatedNormal.x = jo_fixed_mult(axisNormal.x, cos) + jo_fixed_mult(cross.x, sin);
        rotatedNormal.y = jo_fixed_mult(axisNormal.y, cos) + jo_fixed_mult(cross.y, sin);
        rotatedNormal.z = jo_fixed_mult(axisNormal.z, cos) + jo_fixed_mult(cross.z, sin);

        // Rotated normal must be normalized before it can be used, to prevent drifting
        ex_Vector_normal(&rotatedNormal);

        jo_pos3D_fixed result;
        result.x = projected.x + jo_fixed_mult(rotatedNormal.x, radius);
        result.y = projected.y + jo_fixed_mult(rotatedNormal.y, radius);
        result.z = projected.z + jo_fixed_mult(rotatedNormal.z, radius);
        return result;
    }
    else
    {
        // If radius (vector length) is 0, point cannot be rotated
        jo_pos3D_fixed result;
        result.x = toRotate->x;
        result.y = toRotate->y;
        result.z = toRotate->z;
        return result;
    }
}
