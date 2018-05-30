#pragma once
#include "mipsasm.h"
#include <string>

void mipsEmulate();

class MipsEmu : public MipsAsm, public Debugger {
private:
	long regs[32], low, high;
	char mem[65536];

	int pc;
public:
	MipsEmu();
	MipsEmu(vector<string> &ids, vector<mipsTokenPrim> &input);
	virtual ~MipsEmu();

	void prepare();
	long getReg(int off);
	long getMem(int off, int length);

	virtual int line();
	virtual void show();
	virtual int step();
	virtual int process();
	virtual void stop();
};

static string indexReg(int index) {
	switch (index) {
	case 0:
		return "zero";
	case 1:
		return "at";
	case 2:
		return "v0";
	case 3:
		return "v1";
	case 4:
		return "a0";
	case 5:
		return "a1";
	case 6:
		return "a2";
	case 7:
		return "a3";
	case 8:
		return "t0";
	case 9:
		return "t1";
	case 10:
		return "t2";
	case 11:
		return "t3";
	case 12:
		return "t4";
	case 13:
		return "t5";
	case 14:
		return "t6";
	case 15:
		return "t7";
	case 16:
		return "s0";
	case 17:
		return "s1";
	case 18:
		return "s2";
	case 19:
		return "s3";
	case 20:
		return "s4";
	case 21:
		return "s5";
	case 22:
		return "s6";
	case 23:
		return "s7";
	case 24:
		return "t8";
	case 25:
		return "t9";
	case 26:
		return "k0";
	case 27:
		return "k1";
	case 28:
		return "gp";
	case 29:
		return "sp";
	case 30:
		return "fp";
	case 31:
		return "ra";
	}
	return "";
}
static void mipsArg(string filename) {
	string output = filename;
	output = string(output.begin(), output.end() - 4);
	output += "_rev.s";
	std::ifstream fin(filename);
	std::ofstream fout(output);

	if (fin.is_open()) {
		int radix;
		string content;
		getline(fin, content);
		content = content.substr(content.rfind('=') + 1);
		radix = atoi(content.data());

		char ch = 0;
		while (ch != '=')fin >> ch;

		vector<int> instArr;
		unsigned long tmpInst;
		while (!fin.eof()) {
			fin >> std::hex >> tmpInst;
			fin >> std::dec >> ch;
			instArr.push_back(tmpInst);
		}

		for (auto inst : instArr) {
			instNode tmp;
			tmp.value = inst;
			switch (tmp["opcode"]) {
			case 0:
				switch (tmp["funct"]) {
				case 0x0:
					fout << "sll $" << indexReg(tmp["rd"]) << ", $" << indexReg(tmp["rt"]) << ", " << tmp["shamt"] << std::endl;
					break;
				case 0x2:
					fout << "srl $" << indexReg(tmp["rd"]) << ", $" << indexReg(tmp["rt"]) << ", " << tmp["shamt"] << std::endl;
					break;
				case 0x3:
					fout << "sra $" << indexReg(tmp["rd"]) << ", $" << indexReg(tmp["rt"]) << ", " << tmp["shamt"] << std::endl;
					break;
				case 0x4:
					fout << "sllv $" << indexReg(tmp["rd"]) << ", $" << indexReg(tmp["rt"]) << ", $" << indexReg(tmp["rs"]) << std::endl;
					break;
				case 0x6:
					fout << "srlv $" << indexReg(tmp["rd"]) << ", $" << indexReg(tmp["rt"]) << ", $" << indexReg(tmp["rs"]) << std::endl;
					break;
				case 0x7:
					fout << "srav $" << indexReg(tmp["rd"]) << ", $" << indexReg(tmp["rt"]) << ", $" << indexReg(tmp["rs"]) << std::endl;
					break;
				case 0x8:
					fout << "jr $" << indexReg(tmp["rs"]) << std::endl;
					break;
				case 0x9:
					fout << "jalr $" << indexReg(tmp["rs"]) << ", $" << indexReg(tmp["rd"]) << std::endl;
					break;
				case 0x10:
					fout << "mfhi $" << indexReg(tmp["rd"]) << std::endl;
					break;
				case 0x11:
					fout << "mthi $" << indexReg(tmp["rs"]) << std::endl;
					break;
				case 0x12:
					fout << "mflo $" << indexReg(tmp["rd"]) << std::endl;
					break;
				case 0x13:
					fout << "mtlo $" << indexReg(tmp["rs"]) << std::endl;
					break;
				case 0x18:
				case 0x19:
					fout << "mult $" << indexReg(tmp["rs"]) << ", $" << indexReg(tmp["rt"]) << std::endl;
					break;
				case 0x1A:
				case 0x1B:
					fout << "div $" << indexReg(tmp["rs"]) << ", $" << indexReg(tmp["rt"]) << std::endl;
					break;
				case 0x20:
				case 0x21:
					fout << "add $" << indexReg(tmp["rd"]) << ", $" << indexReg(tmp["rs"]) << ", $" << indexReg(tmp["rt"]) << std::endl;
					break;
				case 0x22:
				case 0x23:
					fout << "sub $" << indexReg(tmp["rd"]) << ", $" << indexReg(tmp["rs"]) << ", $" << indexReg(tmp["rt"]) << std::endl;
					break;
				case 0x24:
					fout << "and $" << indexReg(tmp["rd"]) << ", $" << indexReg(tmp["rs"]) << ", $" << indexReg(tmp["rt"]) << std::endl;
					break;
				case 0x25:
					fout << "or $" << indexReg(tmp["rd"]) << ", $" << indexReg(tmp["rs"]) << ", $" << indexReg(tmp["rt"]) << std::endl;
					break;
				case 0x26:
					fout << "xor $" << indexReg(tmp["rd"]) << ", $" << indexReg(tmp["rs"]) << ", $" << indexReg(tmp["rt"]) << std::endl;
					break;
				case 0x27:
					fout << "nor $" << indexReg(tmp["rd"]) << ", $" << indexReg(tmp["rs"]) << ", $" << indexReg(tmp["rt"]) << std::endl;
					break;
				case 0x2a:
				case 0x2b:
					fout << "slt $" << indexReg(tmp["rd"]) << ", $" << indexReg(tmp["rs"]) << ", $" << indexReg(tmp["rt"]) << std::endl;
					break;
				default:
					fout << tmp.value << std::endl;
					break;
				}
				break;
			case 1:
				if (tmp["rt"] == 0) {
					fout << "bltz $" << indexReg(tmp["rs"]) << ", " << tmp["imm"] << std::endl;
				}
				else {
					fout << "bgez $" << indexReg(tmp["rs"]) << ", " << tmp["imm"] << std::endl;
				}
				break;
			case 2:
				fout << "j " << tmp["target"] * 4 << std::endl;
				break;
			case 3:
				fout << "jal " << tmp["target"] * 4 << std::endl;
				break;
			case 4:
				fout << "beq $" << indexReg(tmp["rs"]) << ", $" << indexReg(tmp["rt"]) << ", " << tmp["imm"] << std::endl;
				break;
			case 5:
				fout << "bne $" << indexReg(tmp["rs"]) << ", $" << indexReg(tmp["rt"]) << ", " << tmp["imm"] << std::endl;
				break;
			case 6:
				fout << "blez $" << indexReg(tmp["rs"]) << ", " << tmp["imm"] << std::endl;
				break;
			case 7:
				fout << "bgtz $" << indexReg(tmp["rs"]) << ", " << tmp["imm"] << std::endl;
				break;
			case 8:
			case 9:
				fout << "addi $" << indexReg(tmp["rt"]) << ", $" << indexReg(tmp["rs"]) << ", " << tmp["imm"] << std::endl;
				break;
			case 10:
			case 11:
				fout << "slti $" << indexReg(tmp["rt"]) << ", $" << indexReg(tmp["rs"]) << ", " << tmp["imm"] << std::endl;
				break;
			case 12:
				fout << "andi $" << indexReg(tmp["rt"]) << ", $" << indexReg(tmp["rs"]) << ", " << tmp["imm"] << std::endl;
				break;
			case 13:
				fout << "ori $" << indexReg(tmp["rt"]) << ", $" << indexReg(tmp["rs"]) << ", " << tmp["imm"] << std::endl;
				break;
			case 14:
				fout << "xori $" << indexReg(tmp["rt"]) << ", $" << indexReg(tmp["rs"]) << ", " << tmp["imm"] << std::endl;
				break;
			case 15:
				fout << "lui $" << indexReg(tmp["rt"]) << ", " << tmp["imm"] << std::endl;
				break;
			case 32:
				fout << "lb $" << indexReg(tmp["rt"]) << ", " << tmp["imm"] << "($" << indexReg(tmp["rs"]) << ")" << std::endl;
				break;
			case 33:
				fout << "lh $" << indexReg(tmp["rt"]) << ", " << tmp["imm"] << "($" << indexReg(tmp["rs"]) << ")" << std::endl;
				break;
			case 35:
				fout << "lw $" << indexReg(tmp["rt"]) << ", " << tmp["imm"] << "($" << indexReg(tmp["rs"]) << ")" << std::endl;
				break;
			case 36:
				fout << "lbu $" << indexReg(tmp["rt"]) << ", " << tmp["imm"] << "($" << indexReg(tmp["rs"]) << ")" << std::endl;
				break;
			case 37:
				fout << "lhu $" << indexReg(tmp["rt"]) << ", " << tmp["imm"] << "($" << indexReg(tmp["rs"]) << ")" << std::endl;
				break;
			case 40:
				fout << "sb $" << indexReg(tmp["rt"]) << ", " << tmp["imm"] << "($" << indexReg(tmp["rs"]) << ")" << std::endl;
				break;
			case 41:
				fout << "sh $" << indexReg(tmp["rt"]) << ", " << tmp["imm"] << "($" << indexReg(tmp["rs"]) << ")" << std::endl;
				break;
			case 43:
				fout << "sw $" << indexReg(tmp["rt"]) << ", " << tmp["imm"] << "($" << indexReg(tmp["rs"]) << ")" << std::endl;
				break;
			default:
				fout << tmp.value << std::endl;
				break;
			}
		}
	}

	fin.close();
	fout.close();
}
