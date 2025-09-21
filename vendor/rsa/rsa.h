/*
 * Copyright (C) 2010 The Chromium OS Authors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *    * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <Common.h>

#define CONFIG_RSA_KEY_SIZE 2048
#define RSANUMBYTES ((CONFIG_RSA_KEY_SIZE) / 8)
#define RSANUMWORDS (RSANUMBYTES / sizeof(u32))

/**
 * RSA public key definition
 */
typedef struct {
    u32 n0inv;           // -1 / n[0] mod 2^32
    u32 n[RSANUMWORDS];  // modulus as little endian array
    u32 rr[RSANUMWORDS]; // R^2 as little endian array
} RSAPublicKey;

/**
 * Verify a SHA256WithRSA PKCS#1 v1.5 signature against an expected SHA-256
 * hash.
 *
 * @param key           RSA public key
 * @param signature     RSA signature
 * @param sha           SHA-256 digest of the content to verify
 * @return True on success.
 */
bool RSAVerify(const RSAPublicKey *key, u8 *signature, const u8 *sha);
