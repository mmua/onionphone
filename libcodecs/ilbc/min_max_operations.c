/* vim: set tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab */

/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

/*
 * This file contains the implementation of functions
 * WebRtcSpl_MaxAbsValueW16()
 * WebRtcSpl_MaxAbsValueW32()
 * WebRtcSpl_MaxValueW16()
 * WebRtcSpl_MaxValueW32()
 * WebRtcSpl_MaxIndexW32()
 * WebRtcSpl_MinIndexW32()
 *
 */

#include "signal_processing_library.h"

#include <stdlib.h>

#if !(defined(WEBRTC_ANDROID) && defined(WEBRTC_ARCH_ARM_NEON))

// Maximum absolute value of word16 vector.
int16_t WebRtcSpl_MaxAbsValueW16(const int16_t * vector, int length)
{
	int i = 0, absolute = 0, maximum = 0;

	if (vector == NULL || length <= 0) {
		return -1;
	}

	for (i = 0; i < length; i++) {
		absolute = abs((int)vector[i]);

		if (absolute > maximum) {
			maximum = absolute;
		}
	}

	// Guard the case for abs(-32768).
	if (maximum > WEBRTC_SPL_WORD16_MAX) {
		maximum = WEBRTC_SPL_WORD16_MAX;
	}

	return (int16_t) maximum;
}

// Maximum absolute value of word32 vector.
int32_t WebRtcSpl_MaxAbsValueW32(const int32_t * vector, int length)
{
	// Use uint32_t for the local variables, to accommodate the return value
	// of abs(0x80000000), which is 0x80000000.

	uint32_t absolute = 0, maximum = 0;
	int i = 0;

	if (vector == NULL || length <= 0) {
		return -1;
	}

	for (i = 0; i < length; i++) {
		absolute = abs((int)vector[i]);
		if (absolute > maximum) {
			maximum = absolute;
		}
	}

	maximum = WEBRTC_SPL_MIN(maximum, WEBRTC_SPL_WORD32_MAX);

	return (int32_t) maximum;
}

// Maximum value of word32 vector.
int32_t WebRtcSpl_MaxValueW32(const int32_t * vector, int length)
{
	int32_t maximum = WEBRTC_SPL_WORD32_MIN;
	int i = 0;

	if (vector == NULL || length <= 0) {
		return maximum;
	}

	for (i = 0; i < length; i++) {
		if (vector[i] > maximum)
			maximum = vector[i];
	}
	return maximum;
}
#endif

// Index of maximum value in a word32 vector.
int WebRtcSpl_MaxIndexW32(const int32_t * vector, int length)
{
	int i = 0, index = 0;
	int32_t maximum = WEBRTC_SPL_WORD32_MIN;

	if (vector == NULL || length <= 0) {
		return -1;
	}

	for (i = 0; i < length; i++) {
		if (vector[i] > maximum) {
			maximum = vector[i];
			index = i;
		}
	}

	return index;
}

// Index of minimum value in a word32 vector.
int WebRtcSpl_MinIndexW32(const int32_t * vector, int length)
{
	int i = 0, index = 0;
	int32_t minimum = WEBRTC_SPL_WORD32_MAX;

	if (vector == NULL || length <= 0) {
		return -1;
	}

	for (i = 0; i < length; i++) {
		if (vector[i] < minimum) {
			minimum = vector[i];
			index = i;
		}
	}

	return index;
}
