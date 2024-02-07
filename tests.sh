tests_ui=(
lui
auipc
jal
jalr
beq
bne
blt
bge
bltu
bgeu
lb
lh
lw
lbu
lhu
sb
sh
sw
addi
slti
sltiu
xori
ori
andi
slli
srli
srai
add
sub
sll
slt
sltu
xor
srl
sra
or
and
# #fence
# #fence_i
# #ecall
# #ebreak
# #csrrw
# #csrrs
# #csrrc
# #csrrwi
# #csrrsi
# #csrrci
)

if [ "${ARCH}" = "64" ]; then
    tests_ui+=(
        addiw
        slliw
        srliw
        sraiw
        addw
        subw
        sllw
        srlw
        sraw
        ld
        lwu
        sd
    )
fi;

tests_ua=(
    lrsc
    amoswap_w
    amoadd_w
    amoxor_w
    amoand_w
    amoor_w
    amomin_w
    amomax_w
    amominu_w
    amomaxu_w
)

if [ "${ARCH}" = "64" ]; then
    tests_ua+=(
        amoswap_d
        amoadd_d
        amoxor_d
        amoand_d
        amoor_d
        amomin_d
        amomax_d
        amominu_d
        amomaxu_d
    )
fi;

tests_um=(
    div
    divu
    mulh
    mulhsu
    mulhu
    mul
    rem
    remu
)

if [ "${ARCH}" = "64" ]; then
    tests_um+=(
        mulw
        divw
        divuw
        remw
        remuw
    )
fi;

# unfortunately not all of these tests even pass in qemu
tests_mi=(
    #access
)

tests_my_tests=(
    csr_test
)

tests_my_c_tests=(
    write_read_special_regs
    ecall_test
    irq_test
    # test_printf
    irq_test_irq_ecall_nested
    irq_test_ecall_irq_nested

    # This is only for out of bound mem test, not intended for qemu compare testing
    # mem_out_of_bound_test
)

tests_compliance=(
    add-01
    addi-01
    and-01
    andi-01
    auipc-01
    beq-01
    bge-01
    bgeu-01
    blt-01
    bltu-01
    bne-01
    fence-01
    jal-01
    jalr-01
    lb-align-01
    lbu-align-01
    lh-align-01
    lhu-align-01
    lui-01
    lw-align-01
    or-01
    ori-01
    sb-align-01
    sh-align-01
    sll-01
    slli-01
    slt-01
    slti-01
    sltiu-01
    sltu-01
    sra-01
    srai-01
    srl-01
    srli-01
    sub-01
    sw-align-01
    xor-01
    xori-01
)

if [ "${ARCH}" = "64" ]; then
    tests_compliance+=(
        addiw-01
        addw-01
        ld-align-01
        lwu-align-01
        sd-align-01
        slliw-01
        sllw-01
        sraiw-01
        sraw-01
        srliw-01
        srlw-01
        subw-01
    )
fi;

tests_pmp_user=(
    pmp_user_simple_program
    pmp_user_na4_access_ok
    pmp_user_na4_access_err
    pmp_user_tor_access_ok
    pmp_user_tor_access_err
    pmp_user_simple_program_mprv_check
)

tests_pmp=(
    pmp_everything_ok
    pmp_mprv_s_mode
    pmp_mprv_m_mode
    pmp_na4_ok
    pmp_na4_err
    pmp_tor_ok
    pmp_tor_err
    pmp_tor_first_entry
    pmp_tor_first_entry_err
    pmp_do_nothing
    pmp_tor_read_err
    pmp_mprv_bit

    # The following three tests are commented because:
    # I think this is a bug in qemu, because writing to a PMP region without W flag seems to do nothin in qemu
    #pmp_tor_write_err

    # This is actually OK, it seems that qemu doesn't trace the "faulty" instruction before jumping to the error handler,
    # whereas riscv_em does and then jumps to the error handler. Eventhough it gets traced it has no effect. So I consider this as OK
    # Actually it is not a consistent behavior in qemu.    
    #pmp_tor_exec_err

    # Same as pmp_tor_exec_err
    #pmp_locked_bit_m_mode
)

tests_priv=(
    priv_privileged_levels
    priv_privileged_levels2
    priv_deleg

    # These tests need to be finished first, calling the pass() function
    #priv_irq_deleg
    #priv_irq_deleg_with_sie_disabled
    #priv_irq_deleg_with_sie_enabled
    #priv_irq_deleg_with_sie_enabled_from_m_mode
    #priv_irq_deleg_with_mie_enabled_from_m_mode
    #priv_irq_with_sie_enabled_from_m_mode
    #priv_irq_with_sie_enabled_from_s_mode

    # Another test which I think is not correct on qemu. This should test 
    # an interrupt during an ecall handler. Which should, at least in my understanding, be possible.
    # However qemu advances the epc wrongly by +8 where it actually should be advanced by +4
    # So until I know better, I assume that riscv_em does it correct and qemu is wrong here.
    # priv_nested_deleg_ecall_irq
)

tests_mmu_supervisor=(
    mmu_supervisor_simple
    mmu_supervisor_mxr
)

tests_mmu_user=(
    mmu_user_simple
)

tests_mmu=(
    mmu_simple
    mmu_sum
    mmu_mxr
)

