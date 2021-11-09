// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py
// RUN: %clang_cc1 -triple aarch64-none-linux-gnu -target-feature +sve -fallow-half-arguments-and-returns -S -O2 -Werror -Wall -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -triple aarch64-none-linux-gnu -target-feature +sve -fallow-half-arguments-and-returns -S -O2 -Werror -Wall -emit-llvm -o - -x c++ %s | FileCheck %s -check-prefix=CPP-CHECK

#include <arm_sve.h>

// CHECK-LABEL: @test_svundef2_s8(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret <vscale x 32 x i8> undef
//
// CPP-CHECK-LABEL: @_Z16test_svundef2_s8v(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    ret <vscale x 32 x i8> undef
//
svint8x2_t test_svundef2_s8()
{
  return svundef2_s8();
}

// CHECK-LABEL: @test_svundef2_s16(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret <vscale x 16 x i16> undef
//
// CPP-CHECK-LABEL: @_Z17test_svundef2_s16v(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    ret <vscale x 16 x i16> undef
//
svint16x2_t test_svundef2_s16()
{
  return svundef2_s16();
}

// CHECK-LABEL: @test_svundef2_s32(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret <vscale x 8 x i32> undef
//
// CPP-CHECK-LABEL: @_Z17test_svundef2_s32v(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    ret <vscale x 8 x i32> undef
//
svint32x2_t test_svundef2_s32()
{
  return svundef2_s32();
}

// CHECK-LABEL: @test_svundef2_s64(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret <vscale x 4 x i64> undef
//
// CPP-CHECK-LABEL: @_Z17test_svundef2_s64v(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    ret <vscale x 4 x i64> undef
//
svint64x2_t test_svundef2_s64()
{
  return svundef2_s64();
}

// CHECK-LABEL: @test_svundef2_u8(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret <vscale x 32 x i8> undef
//
// CPP-CHECK-LABEL: @_Z16test_svundef2_u8v(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    ret <vscale x 32 x i8> undef
//
svuint8x2_t test_svundef2_u8()
{
  return svundef2_u8();
}

// CHECK-LABEL: @test_svundef2_u16(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret <vscale x 16 x i16> undef
//
// CPP-CHECK-LABEL: @_Z17test_svundef2_u16v(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    ret <vscale x 16 x i16> undef
//
svuint16x2_t test_svundef2_u16()
{
  return svundef2_u16();
}

// CHECK-LABEL: @test_svundef2_u32(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret <vscale x 8 x i32> undef
//
// CPP-CHECK-LABEL: @_Z17test_svundef2_u32v(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    ret <vscale x 8 x i32> undef
//
svuint32x2_t test_svundef2_u32()
{
  return svundef2_u32();
}

// CHECK-LABEL: @test_svundef2_u64(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret <vscale x 4 x i64> undef
//
// CPP-CHECK-LABEL: @_Z17test_svundef2_u64v(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    ret <vscale x 4 x i64> undef
//
svuint64x2_t test_svundef2_u64()
{
  return svundef2_u64();
}

// CHECK-LABEL: @test_svundef2_f16(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret <vscale x 16 x half> undef
//
// CPP-CHECK-LABEL: @_Z17test_svundef2_f16v(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    ret <vscale x 16 x half> undef
//
svfloat16x2_t test_svundef2_f16()
{
  return svundef2_f16();
}

// CHECK-LABEL: @test_svundef2_f32(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret <vscale x 8 x float> undef
//
// CPP-CHECK-LABEL: @_Z17test_svundef2_f32v(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    ret <vscale x 8 x float> undef
//
svfloat32x2_t test_svundef2_f32()
{
  return svundef2_f32();
}

// CHECK-LABEL: @test_svundef2_f64(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret <vscale x 4 x double> undef
//
// CPP-CHECK-LABEL: @_Z17test_svundef2_f64v(
// CPP-CHECK-NEXT:  entry:
// CPP-CHECK-NEXT:    ret <vscale x 4 x double> undef
//
svfloat64x2_t test_svundef2_f64()
{
  return svundef2_f64();
}