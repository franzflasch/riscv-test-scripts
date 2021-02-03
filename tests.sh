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
