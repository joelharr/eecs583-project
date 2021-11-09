; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=arm64-eabi | FileCheck %s


define <4 x i16> @fptosi_v4f64_to_v4i16(<4 x double>* %ptr) {
; CHECK-LABEL: fptosi_v4f64_to_v4i16:
; CHECK:       // %bb.0:
; CHECK-NEXT:    ldp q0, q1, [x0]
; CHECK-NEXT:    fcvtzs v0.2d, v0.2d
; CHECK-NEXT:    fcvtzs v1.2d, v1.2d
; CHECK-NEXT:    xtn v0.2s, v0.2d
; CHECK-NEXT:    xtn v1.2s, v1.2d
; CHECK-NEXT:    uzp1 v0.4h, v0.4h, v1.4h
; CHECK-NEXT:    ret
  %tmp1 = load <4 x double>, <4 x double>* %ptr
  %tmp2 = fptosi <4 x double> %tmp1 to <4 x i16>
  ret <4 x i16> %tmp2
}

define <8 x i8> @fptosi_v4f64_to_v4i8(<8 x double>* %ptr) {
; CHECK-LABEL: fptosi_v4f64_to_v4i8:
; CHECK:       // %bb.0:
; CHECK-NEXT:    ldp q0, q1, [x0, #32]
; CHECK-NEXT:    fcvtzs v0.2d, v0.2d
; CHECK-NEXT:    ldp q2, q3, [x0]
; CHECK-NEXT:    fcvtzs v1.2d, v1.2d
; CHECK-NEXT:    xtn v0.2s, v0.2d
; CHECK-NEXT:    fcvtzs v2.2d, v2.2d
; CHECK-NEXT:    xtn v1.2s, v1.2d
; CHECK-NEXT:    fcvtzs v3.2d, v3.2d
; CHECK-NEXT:    uzp1 v0.4h, v0.4h, v1.4h
; CHECK-NEXT:    xtn v2.2s, v2.2d
; CHECK-NEXT:    xtn v3.2s, v3.2d
; CHECK-NEXT:    uzp1 v1.4h, v2.4h, v3.4h
; CHECK-NEXT:    uzp1 v0.8b, v1.8b, v0.8b
; CHECK-NEXT:    ret
  %tmp1 = load <8 x double>, <8 x double>* %ptr
  %tmp2 = fptosi <8 x double> %tmp1 to <8 x i8>
  ret <8 x i8> %tmp2
}

define <4 x half> @uitofp_v4i64_to_v4f16(<4 x i64>* %ptr) {
; CHECK-LABEL: uitofp_v4i64_to_v4f16:
; CHECK:       // %bb.0:
; CHECK-NEXT:    ldp q0, q1, [x0]
; CHECK-NEXT:    ucvtf v0.2d, v0.2d
; CHECK-NEXT:    ucvtf v1.2d, v1.2d
; CHECK-NEXT:    fcvtn v0.2s, v0.2d
; CHECK-NEXT:    fcvtn2 v0.4s, v1.2d
; CHECK-NEXT:    fcvtn v0.4h, v0.4s
; CHECK-NEXT:    ret
  %tmp1 = load <4 x i64>, <4 x i64>* %ptr
  %tmp2 = uitofp <4 x i64> %tmp1 to <4 x half>
  ret <4 x half> %tmp2
}

define <4 x i16> @trunc_v4i64_to_v4i16(<4 x i64>* %ptr) {
; CHECK-LABEL: trunc_v4i64_to_v4i16:
; CHECK:       // %bb.0:
; CHECK-NEXT:    ldp q0, q1, [x0]
; CHECK-NEXT:    xtn v0.2s, v0.2d
; CHECK-NEXT:    xtn2 v0.4s, v1.2d
; CHECK-NEXT:    xtn v0.4h, v0.4s
; CHECK-NEXT:    ret
  %tmp1 = load <4 x i64>, <4 x i64>* %ptr
  %tmp2 = trunc <4 x i64> %tmp1 to <4 x i16>
  ret <4 x i16> %tmp2
}

define <4 x i16> @fptoui_v4f64_to_v4i16(<4 x double>* %ptr) {
; CHECK-LABEL: fptoui_v4f64_to_v4i16:
; CHECK:       // %bb.0:
; CHECK-NEXT:    ldp q0, q1, [x0]
; CHECK-NEXT:    fcvtzs v0.2d, v0.2d
; CHECK-NEXT:    fcvtzs v1.2d, v1.2d
; CHECK-NEXT:    xtn v0.2s, v0.2d
; CHECK-NEXT:    xtn v1.2s, v1.2d
; CHECK-NEXT:    uzp1 v0.4h, v0.4h, v1.4h
; CHECK-NEXT:    ret
  %tmp1 = load <4 x double>, <4 x double>* %ptr
  %tmp2 = fptoui <4 x double> %tmp1 to <4 x i16>
  ret <4 x i16> %tmp2
}