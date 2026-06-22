/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Tobias Stoeckmann
 * All rights reserved.
 */
#include "test.h"

/* Test u arg - don't override newer files */
DEFINE_TEST(test_u)
{
	const char *reffile1 = "test_f_new.zip";
	const char *reffile2 = "test_f_old.zip";
	int r;

	extract_reference_file(reffile1);
	extract_reference_file(reffile2);

	/* First run with -u should create file */
	r = systemf("%s -u %s >test.out 2>test.err", testprog, reffile1);
	assertEqualInt(0, r);
	assertNonEmptyFile("test.out");
	assertEmptyFile("test.err");
	assertTextFileContents("world\n", "test.txt");

	/* Second run with -u should not override newer file */
	r = systemf("%s -u %s >test.out 2>test.err", testprog, reffile2);
	assertEqualInt(0, r);
	assertNonEmptyFile("test.out");
	assertEmptyFile("test.err");
	assertTextFileContents("world\n", "test.txt");
}
