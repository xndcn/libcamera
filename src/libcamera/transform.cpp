/* SPDX-License-Identifier: LGPL-2.1-or-later */
/*
 * Copyright (C) 2020, Raspberry Pi (Trading) Limited
 *
 * transform.cpp - 2D plane transforms.
 */

#include <libcamera/transform.h>

/**
 * \file transform.h
 * \brief Enum to represent and manipulate 2D plane transforms
 */

namespace libcamera {

/**
 * \enum Transform
 * \brief Enum to represent a 2D plane transform
 *
 * The Transform can take 8 distinct values, representing the usual 2D plane
 * transforms listed below. Each of these transforms can be constructed
 * out of 3 basic operations, namely a horizontal flip (mirror), a vertical
 * flip, and a transposition (about the main diagonal). The transforms are
 * encoded such that a single bit indicates the presence of each of the 3
 * basic operations:
 *
 * - bit 0 - presence of a horizontal flip
 * - bit 1 - presence of a vertical flip
 * - bit 2 - presence of a transposition.
 *
 * We regard these 3 basic operations as being applied in a specific order:
 * first the two flip operations (actually they commute, so the order between
 * them is unimportant) and finally any transpose operation.
 *
 * Functions are provided to manipulate directly the bits within the transform
 * encoding, but there are also higher-level functions to invert and compose
 * transforms. Transforms are composed according to the usual mathematical
 * convention such that the right transform is applied first, and the left
 * transform is applied second.
 *
 * Finally, we have a total of 8 distinct transformations, as follows (a
 * couple of them have additional synonyms for convenience). We illustrate each
 * with its nominal effect on a rectangle with vertices labelled A, B, C and D.
 *
 * **Identity**
 *
 * Identity transform.
~~~
              A-B                          A-B
Input image   | |   goes to output image   | |
              C-D                          C-D
~~~
 * Numeric value: 0 (no bits set).
 *
 * **Rot0**
 *
 * Synonym for `Identity` (zero degree rotation).
 *
 * **HFlip**
 *
 * Horizontal flip.
~~~
              A-B                          B-A
Input image   | |   goes to output image   | |
              C-D                          D-C
~~~
 * Numeric value: 1 (horizontal flip bit set only).
 *
 * **VFlip**
 *
 * Vertical flip.
~~~
              A-B                          C-D
Input image   | |   goes to output image   | |
              C-D                          A-B
~~~
 * Numeric value: 2 (vertical flip bit set only).
 *
 * **HVFlip**
 *
 * Horizontal and vertical flip (identical to a 180 degree rotation).
~~~
              A-B                          D-C
Input image   | |   goes to output image   | |
              C-D                          B-A
~~~
 * Numeric value: 3 (horizontal and vertical flip bits set).
 *
 * **Rot180**
 *
 * Synonym for `HVFlip` (180 degree rotation).
 *
 * **Transpose**
 *
 * Transpose (about the main diagonal).
~~~
              A-B                          A-C
Input image   | |   goes to output image   | |
              C-D                          B-D
~~~
 * Numeric value: 4 (transpose bit set only).
 *
 * **Rot270**
 *
 * Rotation by 270 degrees clockwise (90 degrees anticlockwise).
~~~
              A-B                          B-D
Input image   | |   goes to output image   | |
              C-D                          A-C
~~~
 * Numeric value: 5 (transpose and horizontal flip bits set).
 *
 * **Rot90**
 *
 * Rotation by 90 degrees clockwise (270 degrees anticlockwise).
~~~
              A-B                          C-A
Input image   | |   goes to output image   | |
              C-D                          D-B
~~~
 * Numeric value: 6 (transpose and vertical flip bits set).
 *
 * **Rot180Transpose**
 *
 * Rotation by 180 degrees followed by transpose (alternatively, transposition
 * about the "opposite diagonal").
~~~
              A-B                          D-B
Input image   | |   goes to output image   | |
              C-D                          C-A
~~~
 * Numeric value: 7 (all bits set).
 *
 * \sa https://en.wikipedia.org/wiki/Examples_of_groups#dihedral_group_of_order_8
 *
 * The set of 2D plane transforms is also known as the symmetry group of a
 * square, described in the link. Note that the group can be generated by
 * only 2 elements (the horizontal flip and a 90 degree rotation, for
 * example), however, the encoding used here makes the presence of the vertical
 * flip explicit.
 */

/**
 * \fn operator &(Transform t0, Transform t1)
 * \brief Apply bitwise AND operator between the bits in the two transforms
 * \param[in] t0 The first transform
 * \param[in] t1 The second transform
 */

/**
 * \fn operator |(Transform t0, Transform t1)
 * \brief Apply bitwise OR operator between the bits in the two transforms
 * \param[in] t0 The first transform
 * \param[in] t1 The second transform
 */

/**
 * \fn operator ^(Transform t0, Transform t1)
 * \brief Apply bitwise XOR operator between the bits in the two transforms
 * \param[in] t0 The first transform
 * \param[in] t1 The second transform
 */

/**
 * \fn operator &=(Transform &t0, Transform t1)
 * \brief Apply bitwise AND-assignment operator between the bits in the two
 * transforms
 * \param[in] t0 The first transform
 * \param[in] t1 The second transform
 */

/**
 * \fn operator |=(Transform &t0, Transform t1)
 * \brief Apply bitwise OR-assignment operator between the bits in the two
 * transforms
 * \param[in] t0 The first transform
 * \param[in] t1 The second transform
 */

/**
 * \fn operator ^=(Transform &t0, Transform t1)
 * \brief Apply bitwise XOR-assignment operator between the bits in the two
 * transforms
 * \param[in] t0 The first transform
 * \param[in] t1 The second transform
 */

/**
 * \brief Compose two transforms together
 * \param[in] t1 The second transform
 * \param[in] t0 The first transform
 *
 * Composing transforms follows the usual mathematical convention for
 * composing functions. That is, when performing `t1 * t0`, \a t0 is applied
 * first, and then \a t1.
 * For example, `Transpose * HFlip` performs `HFlip` first and then the
 * `Transpose` yielding `Rot270`, as shown below.
~~~
             A-B                 B-A                     B-D
Input image  | |   -> HFLip ->   | |   -> Transpose ->   | |   = Rot270
             C-D                 D-C                     A-C
~~~
 * Note that composition is generally non-commutative for Transforms,
 * and not the same as XOR-ing the underlying bit representations.
 */
Transform operator*(Transform t1, Transform t0)
{
	/*
	 * Reorder the operations so that we imagine doing t0's transpose
	 * (if any) after t1's flips. The effect is to swap t1's hflips for
	 * vflips and vice versa, after which we can just xor all the bits.
	 */
	Transform reordered = t1;
	if (!!(t0 & Transform::Transpose)) {
		reordered = t1 & Transform::Transpose;
		if (!!(t1 & Transform::HFlip))
			reordered |= Transform::VFlip;
		if (!!(t1 & Transform::VFlip))
			reordered |= Transform::HFlip;
	}

	return reordered ^ t0;
}

/**
 * \brief Invert a transform
 * \param[in] t The transform to be inverted
 *
 * That is, we return the transform such that `t * (-t)` and `(-t) * t` both
 * yield the identity transform.
 */
Transform operator-(Transform t)
{
	/* All are self-inverses, except for Rot270 and Rot90. */
	static const Transform inverses[] = {
		Transform::Identity,
		Transform::HFlip,
		Transform::VFlip,
		Transform::HVFlip,
		Transform::Transpose,
		Transform::Rot90,
		Transform::Rot270,
		Transform::Rot180Transpose
	};

	return inverses[static_cast<int>(t)];
}

/**
 * \fn operator!(Transform t)
 * \brief Return `true` if the transform is the `Identity`, otherwise `false`
 * \param[in] t The transform to be tested
 */

/**
 * \fn operator~(Transform t)
 * \brief Return the transform with all the bits inverted individually
 * \param[in] t The transform of which the bits will be inverted
 *
 * This inverts the bits that encode the transform in a bitwise manner. Note
 * that this is not the proper inverse of transform \a t (for which use \a
 * operator-).
 */

/**
 * \brief Return the transform representing a rotation of the given angle
 * clockwise
 * \param[in] angle The angle of rotation in a clockwise sense. Negative values
 * can be used to represent anticlockwise rotations
 * \param[out] success Set to `true` if the angle is a multiple of 90 degrees,
 * otherwise `false`
 * \return The transform corresponding to the rotation if \a success was set to
 * `true`, otherwise the `Identity` transform
 */
Transform transformFromRotation(int angle, bool *success)
{
	angle = angle % 360;
	if (angle < 0)
		angle += 360;

	if (success != nullptr)
		*success = true;

	switch (angle) {
	case 0:
		return Transform::Identity;
	case 90:
		return Transform::Rot90;
	case 180:
		return Transform::Rot180;
	case 270:
		return Transform::Rot270;
	}

	if (success != nullptr)
		*success = false;

	return Transform::Identity;
}

/**
 * \brief Return a character string describing the transform
 * \param[in] t The transform to be described.
 */
const char *transformToString(Transform t)
{
	static const char *strings[] = {
		"identity",
		"hflip",
		"vflip",
		"hvflip",
		"transpose",
		"rot270",
		"rot90",
		"rot180transpose"
	};

	return strings[static_cast<int>(t)];
}

} /* namespace libcamera */
