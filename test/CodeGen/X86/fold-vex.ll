; Use CPU parameters to ensure that a CPU-specific attribute is not overriding the AVX definition.

;CHECK: @test
; No need to load from memory. The operand will be loaded as part of the AND instr.
;CHECK-NOT: vmovaps
;CHECK: vandps
;CHECK: ret

; No need to load unaligned operand from memory using an explicit instruction with AVX.
; The operand should be folded into the AND instr.

; With SSE, folding memory operands into math/logic ops requires 16-byte alignment
; unless specially configured on some CPUs such as AMD Family 10H.

define <4 x i32> @test1(<4 x i32>* %p0, <4 x i32> %in1) nounwind {
  %in0 = load <4 x i32>* %p0, align 2
  %a = and <4 x i32> %in0, %in1
  ret <4 x i32> %a

; CHECK-LABEL: @test1
; CHECK-NOT:   vmovups
; CHECK:       vandps (%rdi), %xmm0, %xmm0
; CHECK-NEXT:  ret

; SSE-LABEL: @test1
; SSE:       movups (%rdi), %xmm1
; SSE-NEXT:  andps %xmm1, %xmm0
; SSE-NEXT:  ret
}

