#ifndef __MATHEX__
#define __MATHEX__

typedef struct 
{
    jo_vector_fixed Origin;
    jo_vector_fixed Up;
    jo_vector_fixed Dir;
} CoordinateSystem;

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
void ex_Get_matrix_from_coordinate_system(const CoordinateSystem * coordSys, FIXED (*result)[3]);

/** @brief Project point onto an axis
 *  @param toProject Point to project
 *  @param origin Axis origin point
 *  @param direction Axis direction
 *  @return Projected point
 */
jo_vector_fixed ex_Project_point_to_axis(const jo_vector_fixed * toProject, const jo_vector_fixed * origin, const jo_vector_fixed * direction);

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
jo_vector_fixed ex_Rotate_point_around_axis(const jo_vector_fixed * toRotate, const jo_fixed rad, const jo_vector_fixed * origin, const jo_vector_fixed * direction);

#endif
