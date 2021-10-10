#ifndef __MATHEX__
#define __MATHEX__

/** @brief Coordinate system definition using 2 vectors and a point
 */
typedef struct 
{
    /* Origin point of the coordinate system */
    jo_pos3D_fixed Origin;

    /* Up direction vector */
    jo_vector_fixed Up;

    /* Forward direction vector */
    jo_vector_fixed Dir;
} ex_CoordinateSystem;

/** @brief Translate point by vector
 *  @param toTranslate Point to translate
 *  @param delta Translation vector
 */
void ex_Translate_point(jo_pos3D_fixed * toTranslate, const jo_vector_fixed * delta);

/** @brief Normalize fixed vector
 *  @param a Vector to normalize
 */
void ex_Vector_normal(jo_vector_fixed * a);

/** @brief Compute cross product of two vectors
 *  @param a First vector
 *  @param b Second vector
 *  @return Cross product of two vectors
 */
jo_vector_fixed ex_Vector_cross(const jo_vector_fixed * a, const jo_vector_fixed * b);


/** @brief Compute dot product of two vectors
 *  @param a First vector
 *  @param b Second vector
 *  @return Cross product of two vectors
 */
jo_fixed ex_Dot_product(const jo_fixed * a, const jo_fixed * b);

/** @brief Get transformation matrix from coordinate system
 *  @param coordSys Coordinate system
 *  @param result Resulting matrix
 */
void ex_Get_matrix_from_coordinate_system(const ex_CoordinateSystem * coordSys, FIXED (*result)[3]);

/** @brief Project point onto an axis
 *  @param toProject Point to project
 *  @param origin Axis origin point
 *  @param direction Axis direction
 *  @return Projected point
 */
jo_pos3D_fixed ex_Project_point_to_axis(const jo_pos3D_fixed * toProject, const jo_pos3D_fixed * origin, const jo_vector_fixed * direction);

/** @brief Rotate vector around axis
 *  @param toRotate Point to rotate
 *  @param rad Angle to rotate by in radians
 *  @param direction Axis direction
 *  @return Rotated vector
 */
jo_vector_fixed ex_Rotate_vector_around_axis(const jo_vector_fixed * toRotate, const jo_fixed rad, const jo_vector_fixed * direction);

/** @brief Rotate point around axis
 *  @param toRotate Point to rotate
 *  @param rad Angle to rotate by in radians
 *  @param origin Axis origin point
 *  @param direction Axis direction
 *  @return Rotated point
 */
jo_pos3D_fixed ex_Rotate_point_around_axis(const jo_pos3D_fixed * toRotate, const jo_fixed rad, const jo_pos3D_fixed * origin, const jo_vector_fixed * direction);

#endif
