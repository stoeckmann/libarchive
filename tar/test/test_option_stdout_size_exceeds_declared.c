/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Tim Kientzle
 * All rights reserved.
 */
#include "test.h"

/*
 * `bsdtar -x` writing to real files is protected by archive_write_disk,
 * which already truncates output at an entry's declared size.  `bsdtar
 * -xO` (stdout) bypasses archive_write_disk entirely and writes
 * archive_read_data_into_fd()'s output straight to fd 1 -- this test
 * exercises that path's own backstop (added directly to
 * archive_read_data_into_fd(), so it applies no matter which archive
 * format is being read) at the bsdtar CLI level.
 *
 * The reference archives are crafted ZIP files with two entries: "first"
 * declares uncompressed_size=10 but its true content is longer
 * ("AAAAAAAAAABBBBBBBBBB" for _stored, 256KiB+6 bytes of
 * "CCCCCCCCCCDDDDDDDDDD..." for _deflate), and "second" is a normal
 * 11-byte "hello world" file, extracted right after "first" fails to
 * confirm the mis-declared entry doesn't disrupt the rest of the
 * archive. This test is deliberately independent of the ZIP reader
 * itself enforcing this boundary: archive_read_data_into_fd() truncates
 * output at exactly the declared 10 bytes on its own, so stdout ends up
 * with "first"'s 10-byte prefix immediately followed by "second".
 */
static void
verify(const char *refname, const char *first10)
{
	char expected[32];

	extract_reference_file(refname);
	failure("bsdtar -xO must report an error for the mis-declared entry");
	assert(systemf("%s -xOf %s >test.out 2>test.err", testprog, refname)
	    != 0);

	strcpy(expected, first10);
	strcat(expected, "hello world");
	assertFileContents(expected, (int)strlen(expected), "test.out");
	assertNonEmptyFile("test.err");
}

DEFINE_TEST(test_option_stdout_size_exceeds_declared)
{
	verify("test_option_stdout_size_exceeds_declared_stored.zip",
	    "AAAAAAAAAA");
	verify("test_option_stdout_size_exceeds_declared_deflate.zip",
	    "CCCCCCCCCC");
}
