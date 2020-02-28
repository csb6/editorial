        addi  $a0, $zero, 1
        add   $t0, $t1, $s0
        addi  $v0, $zero, 11
        addiu $s0, $t0, 56
        addu  $s0, $t0, $t0
        and   $t0, $t1, $s4
        andi  $t5, $zero, $s4
        beq   $t5, $s2, Label
        bne   $t5, $s3, Label
        div   $t4, $s4, $s4
        j     Label
        jal   Label
        jr    $s5
        lb    $s6, 0($t5)
        lw    $s6, 4($t5)
        mult  $t7, $zero, $zero
        ori   $s4, $zero, 5
        or    $s3, $zero, $zero
        sb    $t6, 0($t7)
        sw    $t6, 4($t7)
        subu  $t8, $t8, $zero
        sub   $t8, $t8, $zero
        sll   $s4, $s4, 2
        srl   $s4, $s4, 2
        sra   $s4, $s4, 2
        syscall
