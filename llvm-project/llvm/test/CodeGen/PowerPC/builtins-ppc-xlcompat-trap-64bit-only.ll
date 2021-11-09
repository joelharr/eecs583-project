; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -verify-machineinstrs -mtriple=powerpc64le-unknown-linux-gnu \
; RUN:   --ppc-asm-full-reg-names -mcpu=pwr8 < %s | FileCheck %s
; RUN: llc -verify-machineinstrs -mtriple=powerpc64-unknown-linux-gnu \
; RUN:   --ppc-asm-full-reg-names -mcpu=pwr7 < %s | FileCheck %s
; RUN: llc -verify-machineinstrs -mtriple=powerpc64-unknown-aix \
; RUN:   --ppc-asm-full-reg-names -mcpu=pwr8 < %s | FileCheck %s

; tdw
declare void @llvm.ppc.tdw(i64 %a, i64 %b, i32 immarg)
define dso_local void @test__tdwlgt(i64 %a, i64 %b) {
; CHECK-LABEL: test__tdwlgt:
; CHECK:       # %bb.0:
; CHECK-NEXT:    tdlgt r3, r4
; CHECK-NEXT:    blr
  call void @llvm.ppc.tdw(i64 %a, i64 %b, i32 1)
  ret void
}

define dso_local void @test__tdwllt(i64 %a, i64 %b) {
; CHECK-LABEL: test__tdwllt:
; CHECK:       # %bb.0:
; CHECK-NEXT:    tdllt r3, r4
; CHECK-NEXT:    blr
  call void @llvm.ppc.tdw(i64 %a, i64 %b, i32 2)
  ret void
}

define dso_local void @test__tdw3(i64 %a, i64 %b) {
; CHECK-LABEL: test__tdw3:
; CHECK:       # %bb.0:
; CHECK-NEXT:    td 3, r3, r4
; CHECK-NEXT:    blr
  call void @llvm.ppc.tdw(i64 %a, i64 %b, i32 3)
  ret void
}
define dso_local void @test__tdweq(i64 %a, i64 %b) {
; CHECK-LABEL: test__tdweq:
; CHECK:       # %bb.0:
; CHECK-NEXT:    tdeq r3, r4
; CHECK-NEXT:    blr
  call void @llvm.ppc.tdw(i64 %a, i64 %b, i32 4)
  ret void
}

define dso_local void @test__tdwlge(i64 %a, i64 %b) {
; CHECK-LABEL: test__tdwlge:
; CHECK:       # %bb.0:
; CHECK-NEXT:    td 5, r3, r4
; CHECK-NEXT:    blr
  call void @llvm.ppc.tdw(i64 %a, i64 %b, i32 5)
  ret void
}

define dso_local void @test__tdwlle(i64 %a, i64 %b) {
; CHECK-LABEL: test__tdwlle:
; CHECK:       # %bb.0:
; CHECK-NEXT:    td 6, r3, r4
; CHECK-NEXT:    blr
  call void @llvm.ppc.tdw(i64 %a, i64 %b, i32 6)
  ret void
}

define dso_local void @test__tdwgt(i64 %a, i64 %b) {
; CHECK-LABEL: test__tdwgt:
; CHECK:       # %bb.0:
; CHECK-NEXT:    tdgt r3, r4
; CHECK-NEXT:    blr
  call void @llvm.ppc.tdw(i64 %a, i64 %b, i32 8)
  ret void
}

define dso_local void @test__tdwge(i64 %a, i64 %b) {
; CHECK-LABEL: test__tdwge:
; CHECK:       # %bb.0:
; CHECK-NEXT:    td 12, r3, r4
; CHECK-NEXT:    blr
  call void @llvm.ppc.tdw(i64 %a, i64 %b, i32 12)
  ret void
}

define dso_local void @test__tdwlt(i64 %a, i64 %b) {
; CHECK-LABEL: test__tdwlt:
; CHECK:       # %bb.0:
; CHECK-NEXT:    tdlt r3, r4
; CHECK-NEXT:    blr
  call void @llvm.ppc.tdw(i64 %a, i64 %b, i32 16)
  ret void
}

define dso_local void @test__tdwle(i64 %a, i64 %b) {
; CHECK-LABEL: test__tdwle:
; CHECK:       # %bb.0:
; CHECK-NEXT:    td 20, r3, r4
; CHECK-NEXT:    blr
  call void @llvm.ppc.tdw(i64 %a, i64 %b, i32 20)
  ret void
}

define dso_local void @test__tdwne24(i64 %a, i64 %b) {
; CHECK-LABEL: test__tdwne24:
; CHECK:       # %bb.0:
; CHECK-NEXT:    tdne r3, r4
; CHECK-NEXT:    blr
  call void @llvm.ppc.tdw(i64 %a, i64 %b, i32 24)
  ret void
}

define dso_local void @test__tdw31(i64 %a, i64 %b) {
; CHECK-LABEL: test__tdw31:
; CHECK:       # %bb.0:
; CHECK-NEXT:    tdu r3, r4
; CHECK-NEXT:    blr
  call void @llvm.ppc.tdw(i64 %a, i64 %b, i32 31)
  ret void
}

define dso_local void @test__tdw_no_match(i64 %a, i64 %b) {
; CHECK-LABEL: test__tdw_no_match:
; CHECK:       # %bb.0:
; CHECK-NEXT:    td 13, r3, r4
; CHECK-NEXT:    blr
  call void @llvm.ppc.tdw(i64 %a, i64 %b, i32 13)
  ret void
}

; trapd
declare void @llvm.ppc.trapd(i64 %a)
define dso_local void @test__trapd(i64 %a) {
; CHECK-LABEL: test__trapd:
; CHECK:       # %bb.0:
; CHECK-NEXT:    tdnei r3, 0
; CHECK-NEXT:    blr
  call void @llvm.ppc.trapd(i64 %a)
  ret void
}