#include "mipsasm.h"
#include "winsgl.h"
#include <iomanip>

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

MipsAsm *MipsAsm::input(vector<string> &ids, vector<mipsTokenPrim> &input) {
	content = input;

	output.clear();
	for (auto i : ids) {
		unsigned int j = 0;
		for (; j < strId.size(); j++) {
			if (i == strId[j])break;
		}
		if (j == strId.size())strId.push_back(i);
	}

	return this;
}
vector<instNode>MipsAsm::parse() {
	proc = 0;
	tmpAddr = 0;
	instNode head = instNode();
	tmpAddr += 4;
	head.op = MIPS_ID_NOP;
	head.offset = 0;
	head.length = 4;
	output.push_back(head);
	
	while (proc < content.size()) {
		if (content[proc].type == MIPS_TT_USER) {
			label.push_back(std::pair<string, int>(strId[content[proc].id], tmpAddr));
			for (auto l : suspend) {
				if (l.first == strId[content[proc].id]) {
					for (auto p : l.second) {
						if (p.first == -2) {
							codeAddr = tmpAddr;
							output[0].setTarget(codeAddr);
						}
						else if (p.first == -1)dataAddr = tmpAddr;
						else {
							switch (output[p.first].op) {
							case MIPS_ID_J:
							case MIPS_ID_JAL:
								output[p.first].setTarget(tmpAddr/4);
								break;
							case MIPS_ID_BEQ:
							case MIPS_ID_BNE:
							case MIPS_ID_BGEZ:
							case MIPS_ID_BGTZ:
							case MIPS_ID_BLEZ:
							case MIPS_ID_BLTZ:
								output[p.first].setImm(tmpAddr - output[p.first].offset);
								break;
							}
						}
					}
				}
			}
			proc++;
			if (content[proc].type == MIPS_TT_OP || content[proc].id == MIPS_OP_COLON)proc++;
			else if (content[proc].type == MIPS_TT_SYS || content[proc].id == MIPS_ID_EQU) {
				proc++;
				if (content[proc].type = MIPS_TT_DATA)
					error("equ", MIPS_SE_INVALIDTYPE);
				label[label.size() - 1].second = (int)content[proc].value;
			}
			else error("label", MIPS_SE_EXPCOLON);
		}
		else if (content[proc].type == MIPS_TT_SYS) {
			instNode tmp;
			tmp.offset = tmpAddr;
			tmp.length = 4;
			tmp.op = (MIPSIDENTIFIER)content[proc].id;
			tmp.line = content[proc].line;
			switch (content[proc].id) {
			case MIPS_ID_DADDR:
				proc++;
				tmp.length = 0;
				if (content[proc].type == MIPS_TT_OP || content[proc].id == MIPS_OP_COLON)proc++;
				else error("DataAddre", MIPS_SE_EXPCOLON);
				dataAddr = parseLabel(MIPS_ID_DADDR);
				break;
			case MIPS_ID_CADDR:
				proc++;
				tmp.length = 0;
				if (content[proc].type == MIPS_TT_OP || content[proc].id == MIPS_OP_COLON)proc++;
				else error("BaseAddre", MIPS_SE_EXPCOLON);
				codeAddr = parseLabel(MIPS_ID_CADDR);
				output[0].op = MIPS_ID_J;
				output[0].setTarget(codeAddr);
				output[0].setOpcode(0x2);
				break;
			case MIPS_ID_DB:
				proc++;
				tmp.length = 1;
				tmp.value = (char)content[proc].value;
				break;
			case MIPS_ID_DW:
				proc++;
				tmp.length = 2;
				tmp.value = (short)content[proc].value;
				break;
			case MIPS_ID_DD:
				proc++;
				tmp.length = 4;
				tmp.value = (int)content[proc].value;
				break;
			case MIPS_ID_LW:
				tmp.setOpcode(0x23);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("lw", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("lw", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type == MIPS_TT_DATA) {
					tmp.setImm((int)content[proc].value);
				}
				else if (content[proc].type != MIPS_TT_SYS) {
					tmp.setImm(parseLabel());
				}
				else
					error("lw", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_LBRACE)
					error("lw", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("lw", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_RBRACE)
					error("lw", MIPS_SE_UNIQUE);
				break;
			case MIPS_ID_LB:
				tmp.setOpcode(0x20);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("lb", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("lb", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type == MIPS_TT_DATA) {
					tmp.setImm((char)content[proc].value);
				}
				else if (content[proc].type != MIPS_TT_SYS) {
					tmp.setImm((char)parseLabel());
				}
				else
					error("lb", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_LBRACE)
					error("lb", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("lb", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_RBRACE)
					error("lb", MIPS_SE_UNIQUE);
				break;
			case MIPS_ID_LBU:
				tmp.setOpcode(0x24);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("lbu", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("lbu", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type == MIPS_TT_DATA) {
					tmp.setImm((char)content[proc].value);
				}
				else if (content[proc].type != MIPS_TT_SYS) {
					tmp.setImm((char)parseLabel());
				}
				else
					error("lbu", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_LBRACE)
					error("lbu", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("lbu", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_RBRACE)
					error("lbu", MIPS_SE_UNIQUE);
				break;
			case MIPS_ID_LH:
				tmp.setOpcode(0x21);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("lh", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("lh", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type == MIPS_TT_DATA) {
					tmp.setImm((short)content[proc].value);
				}
				else if (content[proc].type != MIPS_TT_SYS) {
					tmp.setImm((short)parseLabel());
				}
				else
					error("lh", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_LBRACE)
					error("lh", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("lh", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_RBRACE)
					error("lh", MIPS_SE_UNIQUE);
				break;
			case MIPS_ID_LHU:
				tmp.setOpcode(0x25);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("lhu", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("lhu", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type == MIPS_TT_DATA) {
					tmp.setImm((short)content[proc].value);
				}
				else if (content[proc].type != MIPS_TT_SYS) {
					tmp.setImm((short)parseLabel());
				}
				else
					error("lhu", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_LBRACE)
					error("lhu", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("lhu", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_RBRACE)
					error("lhu", MIPS_SE_UNIQUE);
				break;
			case MIPS_ID_SB:
				tmp.setOpcode(0x28);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sb", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("sb", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type == MIPS_TT_DATA) {
					tmp.setImm((char)content[proc].value);
				}
				else if (content[proc].type != MIPS_TT_SYS) {
					tmp.setImm((char)parseLabel());
				}
				else
					error("sb", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_LBRACE)
					error("sb", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sb", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_RBRACE)
					error("sb", MIPS_SE_UNIQUE);
				break;
			case MIPS_ID_SH:
				tmp.setOpcode(0x29);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sh", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("sh", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type == MIPS_TT_DATA) {
					tmp.setImm((short)content[proc].value);
				}
				else if (content[proc].type != MIPS_TT_SYS) {
					tmp.setImm((short)parseLabel());
				}
				else
					error("sh", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_LBRACE)
					error("sh", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sh", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_RBRACE)
					error("sh", MIPS_SE_UNIQUE);
				break;
			case MIPS_ID_SW:
				tmp.setOpcode(0x2B);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sw", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("sw", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type == MIPS_TT_DATA) {
					tmp.setImm((int)content[proc].value);
				}
				else if (content[proc].type != MIPS_TT_SYS) {
					tmp.setImm(parseLabel());
				}
				else
					error("sw", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_LBRACE)
					error("sw", MIPS_SE_UNIQUE);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sw", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_RBRACE)
					error("sw", MIPS_SE_UNIQUE);
				break;
			case MIPS_ID_ADD:
				tmp.setFunct(0x20);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("add", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("add", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("add", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("add", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("add", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				break;
			case MIPS_ID_ADDU:
				tmp.setFunct(0x21);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("addu", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("addu", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("addu", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("addu", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("addu", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				break;
			case MIPS_ID_ADDS: {
				instNode step[5];
				break;
			}
			case MIPS_ID_SUB:
				tmp.setFunct(0x22);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sub", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("sub", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sub", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("sub", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sub", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				break;
			case MIPS_ID_SUBU:
				tmp.setFunct(0x23);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("subu", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("subu", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("subu", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("subu", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("subu", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				break;
			case MIPS_ID_SLT:
				tmp.setFunct(0x2A);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("slt", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("slt", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("slt", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("slt", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("slt", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				break;
			case MIPS_ID_SLTU:
				tmp.setFunct(0x2B);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sltu", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("sltu", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sltu", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("sltu", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sltu", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				break;
			case MIPS_ID_AND:
				tmp.setFunct(0x24);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("and", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("and", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("and", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("and", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("and", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				break;
			case MIPS_ID_OR:
				tmp.setFunct(0x25);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("or", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("or", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("or", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("or", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("or", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				break;
			case MIPS_ID_XOR:
				tmp.setFunct(0x26);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("xor", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("xor", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("xor", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("xor", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("xor", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				break;
			case MIPS_ID_NOR:
				tmp.setFunct(0x27);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("nor", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("nor", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("nor", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("nor", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("nor", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				break;
			case MIPS_ID_SLL:
				tmp.setFunct(0x0);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sll", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("sll", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sll", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("sll", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_DATA)
					error("sll", MIPS_SE_UNIQUE);
				tmp.setShamt((int)content[proc].value);
				break;
			case MIPS_ID_SLLV:
				tmp.setFunct(0x4);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sllv", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("sllv", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sllv", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("sllv", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sllv", MIPS_SE_EXPREG);
				tmp.setRs((int)content[proc].id);
				break;
			case MIPS_ID_SRL:
				tmp.setFunct(0x2);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("srl", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("srl", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("srl", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("srl", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_DATA)
					error("srl", MIPS_SE_UNIQUE);
				tmp.setShamt((int)content[proc].value);
				break;
			case MIPS_ID_SRLV:
				tmp.setFunct(0x6);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("srlv", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("srlv", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("srlv", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("srlv", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("srlv", MIPS_SE_EXPREG);
				tmp.setRs((int)content[proc].id);
				break;
			case MIPS_ID_SRA:
				tmp.setFunct(0x3);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sra", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("sra", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sra", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("sra", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_DATA)
					error("sra", MIPS_SE_UNIQUE);
				tmp.setShamt((int)content[proc].value);
				break;
			case MIPS_ID_SRAV:
				tmp.setFunct(0x7);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("srav", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("srav", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("srav", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("srav", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("srav", MIPS_SE_EXPREG);
				tmp.setRs((int)content[proc].id);
				break;
			case MIPS_ID_MULT:
				tmp.setFunct(0x18);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("mult", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("mult", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("mult", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				break;
			case MIPS_ID_MULTU:
				tmp.setFunct(0x19);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("multu", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("multu", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("multu", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				break;
			case MIPS_ID_MULTS:
				break;
			case MIPS_ID_DIV:
				tmp.setFunct(0x1A);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("div", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("div", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("div", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				break;
			case MIPS_ID_DIVU:
				tmp.setFunct(0x1B);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("divu", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("divu", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("divu", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				break;
			case MIPS_ID_ADDI:
				tmp.setOpcode(0x8);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("addi", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("addi", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("addi", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("addi", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_DATA)
					error("addi", MIPS_SE_UNIQUE);
				tmp.setImm((int)content[proc].value);
				break;
			case MIPS_ID_ADDIU:
				tmp.setOpcode(0x9);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("addiu", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("addiu", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("addiu", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("addiu", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_DATA)
					error("addiu", MIPS_SE_UNIQUE);
				tmp.setImm((int)content[proc].value);
				break;
			case MIPS_ID_ANDI:
				tmp.setOpcode(0xC);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("andi", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("andi", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("andi", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("andi", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_DATA)
					error("andi", MIPS_SE_UNIQUE);
				tmp.setImm((int)content[proc].value);
				break;
			case MIPS_ID_ORI:
				tmp.setOpcode(0xD);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("ori", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("ori", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("ori", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("ori", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_DATA)
					error("ori", MIPS_SE_UNIQUE);
				tmp.setImm((int)content[proc].value);
				break;
			case MIPS_ID_XORI:
				tmp.setOpcode(0xE);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("xori", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("xori", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("xori", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("xori", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_DATA)
					error("xori", MIPS_SE_UNIQUE);
				tmp.setImm((int)content[proc].value);
				break;
			case MIPS_ID_LUI:
				tmp.setOpcode(0xF);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("lui", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("lui", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_DATA)
					error("lui", MIPS_SE_UNIQUE);
				tmp.setImm((int)content[proc].value);
				break;
			case MIPS_ID_SLTI:
				tmp.setOpcode(0xA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("slti", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("slti", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("slti", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("slti", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_DATA)
					error("slti", MIPS_SE_UNIQUE);
				tmp.setImm((int)content[proc].value);
				break;
			case MIPS_ID_SLTIU:
				tmp.setOpcode(0xB);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sltiu", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("sltiu", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("sltiu", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("sltiu", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_DATA)
					error("sltiu", MIPS_SE_UNIQUE);
				tmp.setImm((int)content[proc].value);
				break;
			case MIPS_ID_BEQ:
				tmp.setOpcode(0x4);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("beq", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("beq", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("beq", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("beq", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type == MIPS_TT_USER)
					tmp.setImm(parseLabel() - tmpAddr);
				else if (content[proc].type == MIPS_TT_DATA)
					tmp.setImm((int)content[proc].value);
				break;
			case MIPS_ID_BNE:
				tmp.setOpcode(0x5);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("bne", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("bne", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("bne", MIPS_SE_EXPREG);
				tmp.setRt(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("bne", MIPS_SE_EXPCOMMA);
				proc++;
				if(content[proc].type == MIPS_TT_USER)
					tmp.setImm(parseLabel() - tmpAddr);
				else if(content[proc].type == MIPS_TT_DATA)
					tmp.setImm((int)content[proc].value);
				break;
			case MIPS_ID_BLEZ:
				tmp.setOpcode(0x6);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("blez", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("blez", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type == MIPS_TT_USER)
					tmp.setImm(parseLabel() - tmpAddr);
				else if (content[proc].type == MIPS_TT_DATA)
					tmp.setImm((int)content[proc].value);
				break;
			case MIPS_ID_BGTZ:
				tmp.setOpcode(0x7);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("bgtz", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("bgtz", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type == MIPS_TT_USER)
					tmp.setImm(parseLabel() - tmpAddr);
				else if (content[proc].type == MIPS_TT_DATA)
					tmp.setImm((int)content[proc].value);
				break;
			case MIPS_ID_BLTZ:
				tmp.setOpcode(0x1);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("bltz", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("bltz", MIPS_SE_EXPCOMMA);
				proc++;
				if (content[proc].type == MIPS_TT_USER)
					tmp.setImm(parseLabel() - tmpAddr);
				else if (content[proc].type == MIPS_TT_DATA)
					tmp.setImm((int)content[proc].value);
				break;
			case MIPS_ID_BGEZ:
				tmp.setOpcode(0x1);
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("bgez", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type != MIPS_TT_OP || content[proc].id != MIPS_OP_COMMA)
					error("bgez", MIPS_SE_EXPCOMMA);
				proc++;
				tmp.setRt(1);
				if (content[proc].type == MIPS_TT_USER)
					tmp.setImm(parseLabel() - tmpAddr);
				else if (content[proc].type == MIPS_TT_DATA)
					tmp.setImm((int)content[proc].value);
				break;
			case MIPS_ID_J:
				tmp.setOpcode(0x2);
				proc++;
				tmp.setImm(parseLabel()/4);
				break;
			case MIPS_ID_JAL:
				tmp.setOpcode(0x3);
				proc++;
				tmp.setImm(parseLabel()/4);
				break;
			case MIPS_ID_JALR:
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("jalr", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				proc++;
				if (content[proc].type == MIPS_TT_OP || content[proc].id == MIPS_OP_COMMA) {
					proc++;
					if (content[proc].type != MIPS_TT_REG)
						error("jalr", MIPS_SE_EXPREG);
					tmp.setRd(content[proc].id);
				}
				else {
					tmp.setRd(31);
				}
				tmp.setFunct(0x9);
				break;
			case MIPS_ID_JR:
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("jr", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				tmp.setFunct(0x8);
				break;
			case MIPS_ID_MFHI:
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("mfhi", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				tmp.setFunct(16);
				break;
			case MIPS_ID_MFLO:
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("mfhi", MIPS_SE_EXPREG);
				tmp.setRd(content[proc].id);
				tmp.setFunct(18);
				break;
			case MIPS_ID_MTHI:
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("mfhi", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				tmp.setFunct(17);
				break;
			case MIPS_ID_MTLO:
				proc++;
				if (content[proc].type != MIPS_TT_REG)
					error("mfhi", MIPS_SE_EXPREG);
				tmp.setRs(content[proc].id);
				tmp.setFunct(19);
				break;
			case MIPS_ID_NOP:
				break;
			default:
				break;
			}
			proc++;
			tmpAddr += tmp.length;
			if(tmp.length)output.push_back(tmp);
		}
	}

	return output;
}
void MipsAsm::write(string filename) {
	std::ofstream fout(filename);

	fout << "memory_initialization_radix = 16;" << std::endl;
	fout << "memory_initialization_vector = " << std::endl;

	char *byte;
	short *word;
	long *dword;
	char mem[65536] = { 0 };
	int total = 0;
	for (auto i : output) {
		switch (i.length) {
		case 1:
			byte = mem + i.offset;
			*byte = i.value;
			total += 1;
			break;
		case 2:
			word = (short *)(mem + i.offset);
			*word = i.value;
			total += 2;
			break;
		case 4:
			dword = (long *)(mem + i.offset);
			*dword = i.value;
			total += 4;
			break;
		}
	}
	for (int i = 0; i < total; i += 4) {
		fout << std::setw(8) << std::setfill('0') << std::hex << *(int *)(mem + i) << ",";
		if ((i + 4) % 32 == 0)fout << std::endl;
	}
	fout << "00000000;" << std::endl;

	fout.close();

	alertInfo("已完成汇编。", "汇编器", ALERT_BUTTON_OK);
}

unsigned int MipsAsm::parseLabel(int mode) {
	if (content[proc].type == MIPS_TT_DATA)return (unsigned int)content[proc].value + 4;
	else if (content[proc].type == MIPS_TT_USER) {
		for (auto &l : label) {
			if (l.first == strId[content[proc].id])return l.second;
		}
		for (auto &l : suspend) {
			if (l.first == strId[content[proc].id]) {
				if(mode == -1)l.second.push_back(std::pair<int, expNode *>(output.size(), NULL));
				else if(mode == MIPS_ID_DADDR)l.second.push_back(std::pair<int, expNode *>(-1, NULL));
				else if (mode == MIPS_ID_CADDR)l.second.push_back(std::pair<int, expNode *>(-2, NULL));
				return 0;
			}
		}
		vector<std::pair<int, expNode *>> unknown;
		if (mode == -1)unknown.push_back(std::pair<int, expNode *>(output.size(), NULL));
		else if (mode == MIPS_ID_DADDR)unknown.push_back(std::pair<int, expNode *>(-1, NULL));
		else if (mode == MIPS_ID_CADDR)unknown.push_back(std::pair<int, expNode *>(-2, NULL));
		suspend.push_back(std::pair<string, vector<std::pair<int, expNode *>>>(strId[content[proc].id], unknown));
		return 0;
	}
	else error("label", MIPS_SE_INCOMPLETE);
	return 0;
}

void MipsAsm::error(const char *inst, int type) {
	switch (type) {
	case MIPS_SE_UNIQUE:
		throw new MIPSyntaxException(std::string(inst) + "无此用法。");
	case MIPS_SE_EXPCOLON:
		throw new MIPSyntaxException(std::string(inst) + "缺少冒号。");
	case MIPS_SE_EXPCOMMA:
		throw new MIPSyntaxException(std::string(inst) + "缺少逗号。");
	case MIPS_SE_EXPREG:
		throw new MIPSyntaxException(std::string(inst) + "请指定寄存器。");
	case MIPS_SE_EXPBRACE:
		throw new MIPSyntaxException(std::string(inst) + "括号不完整。");
	case MIPS_SE_REDEF:
		throw new MIPSyntaxException(std::string(inst) + "重定义。");
	case MIPS_SE_INVALIDTYPE:
		throw new MIPSyntaxException(std::string(inst) + "操作对象类型错误。");
	case MIPS_SE_DISACCORD:
		throw new MIPSyntaxException(std::string(inst) + "前后不一致。");
	case MIPS_SE_NOID:
		throw new MIPSyntaxException(std::string(inst) + "未定义。");
	case MIPS_SE_INCOMPLETE:
		throw new MIPSyntaxException(std::string(inst) + "语句不完整。");
	case MIPS_SE_UNKNOWN:
		throw new MIPSyntaxException(std::string(inst) + "未知错误。");
	case MIPS_SE_UNSUPPORT:
		throw new MIPSyntaxException(std::string(inst) + "暂不支持。");
	}
}
