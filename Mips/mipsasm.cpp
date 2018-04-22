#include "mipsasm.h"

MipsAsm::MipsAsm() {
	proc = 0;

	output.clear();
}
MipsAsm::MipsAsm(vector<string> &ids, vector<mipsTokenPrim> &input) {
	proc = 0;

	content = input;
	strId = ids;
	output.clear();
}
MipsAsm::~MipsAsm() {

}

void MipsAsm::input(vector<string> &ids, vector<mipsTokenPrim> &input) {
	content = input;

	output.clear();
	for (auto i : ids) {
		unsigned int j = 0;
		for (; j < strId.size(); j++) {
			if (i == strId[j])break;
		}
		if (j == strId.size())strId.push_back(i);
	}
}
vector<instNode>MipsAsm::parse() {
	proc = 0;
	while (proc < content.size()) {
		if (content[proc].type == MIPS_TT_USER) {
			label.push_back(std::pair<string, int>(strId[content[proc++].id], output.size()));
		}
		else if (content[proc].type == MIPS_TT_SYS) {
			switch (content[proc].id) {
			case MIPS_ID_DADDR:
				break;
			case MIPS_ID_CADDR:
				break;
			case MIPS_ID_DB:
				break;
			case MIPS_ID_DW:
				break;
			case MIPS_ID_DD:
				break;
			case MIPS_ID_EQU:
				break;
			case MIPS_ID_LW:
				break;
			case MIPS_ID_LB:
				break;
			case MIPS_ID_LBU:
				break;
			case MIPS_ID_LH:
				break;
			case MIPS_ID_LHU:
				break;
			case MIPS_ID_SB:
				break;
			case MIPS_ID_SH:
				break;
			case MIPS_ID_SW:
				break;
			case MIPS_ID_ADD:
				break;
			case MIPS_ID_ADDU:
				break;
			case MIPS_ID_SUB:
				break;
			case MIPS_ID_SUBU:
				break;
			case MIPS_ID_SLT:
				break;
			case MIPS_ID_SLTU:
				break;
			case MIPS_ID_AND:
				break;
			case MIPS_ID_OR:
				break;
			case MIPS_ID_XOR:
				break;
			case MIPS_ID_NOR:
				break;
			case MIPS_ID_SLL:
				break;
			case MIPS_ID_SRL:
				break;
			case MIPS_ID_SRA:
				break;
			case MIPS_ID_MULT:
				break;
			case MIPS_ID_MULTU:
				break;
			case MIPS_ID_DIV:
				break;
			case MIPS_ID_DIVU:
				break;
			case MIPS_ID_ADDI:
				break;
			case MIPS_ID_ADDIU:
				break;
			case MIPS_ID_ANDI:
				break;
			case MIPS_ID_ORI:
				break;
			case MIPS_ID_XORI:
				break;
			case MIPS_ID_LUI:
				break;
			case MIPS_ID_SLTI:
				break;
			case MIPS_ID_SLTIU:
				break;
			case MIPS_ID_BEQ:
				break;
			case MIPS_VID_BNE:
				break;
			case MIPS_ID_BLEZ:
				break;
			case MIPS_ID_BGTZ:
				break;
			case MIPS_ID_BLTZ:
				break;
			case MIPS_ID_BGEZ:
				break;
			case MIPS_ID_J:
				break;
			case MIPS_ID_JAL:
				break;
			case MIPS_ID_JALR:
				break;
			case MIPS_ID_JR:
				break;
			default:
				break;
			}
		}
	}

	return output;
}
void MipsAsm::write(string filename) {

}