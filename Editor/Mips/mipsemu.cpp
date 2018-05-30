#define _CRT_SECURE_NO_WARNINGS
#include "editor/editor.h"
#include "mipsemu.h"

extern Editor *editor;

MipsEmu::MipsEmu() {

}
MipsEmu::MipsEmu(vector<string> &ids, vector<mipsTokenPrim> &input) : MipsAsm(ids, input) {

}
MipsEmu::~MipsEmu() {

}

void MipsEmu::prepare() {
	char *byte;
	short *word;
	long *dword;
	for (auto i : output) {
		switch (i.length) {
		case 1:
			byte = mem + i.offset;
			*byte = i.value;
			break;
		case 2:
			word = (short *)(mem + i.offset);
			*word = i.value;
			break;
		case 4:
			dword = (long *)(mem + i.offset);
			*dword = i.value;
			break;
		}
	}
	pc = 0;
}
long MipsEmu::getReg(int off) {
	return regs[off];
}
long MipsEmu::getMem(int off, int length) {
	switch (length) {
	case 1:
		return mem[off];
	case 2:
		return *(short *)(mem + off);
	case 4:
		return *(long *)(mem + off);
		break;
	}
	return 0;
}

int MipsEmu::line() {
	return 0;
}
void MipsEmu::show() {
	int baseX = editor->editorBase.x + editor->editorSize.x;
	int baseY = editor->editorBase.y - 20;
	int sizeX = DEBUGGER_WIDTH;
	int sizeY = editor->editorSize.y + 20;

	setColor(223, 223, 223);
	putQuad(baseX, baseY, baseX + sizeX, baseY + sizeY, SOLID_FILL);

	char hex[9];
	setColor(0, 0, 0);
	setFontSize(20);
	setFontName("Î¢ÈíÑÅºÚ");
	putString("pc:", baseX + 40, baseY + 60);
	sprintf(hex, "%x", pc);
	putString(hex, baseX + 72, baseY + 60);
	putString("inst:", baseX + 40, baseY + 80);
	sprintf(hex, "%08x", getMem(pc, 4));
	putString(hex, baseX + 80, baseY + 80);

	setColor(63, 63, 63);
	putString("$zero:", baseX + 42, baseY + 100);
	putString("$at:", baseX + 162, baseY + 100);
	putString("$v0:", baseX + 42, baseY + 120);
	putString("$v1:", baseX + 162, baseY + 120);
	putString("$a0:", baseX + 42, baseY + 140);
	putString("$a1:", baseX + 162, baseY + 140);
	putString("$a2:", baseX + 42, baseY + 160);
	putString("$a3:", baseX + 162, baseY + 160);
	putString("$t0:", baseX + 42, baseY + 180);
	putString("$t1:", baseX + 162, baseY + 180);
	putString("$t2:", baseX + 42, baseY + 200);
	putString("$t3:", baseX + 162, baseY + 200);
	putString("$t4:", baseX + 42, baseY + 220);
	putString("$t5:", baseX + 162, baseY + 220);
	putString("$t6:", baseX + 42, baseY + 240);
	putString("$t7:", baseX + 162, baseY + 240);
	putString("$s0:", baseX + 42, baseY + 260);
	putString("$s1:", baseX + 162, baseY + 260);
	putString("$s2:", baseX + 42, baseY + 280);
	putString("$s3:", baseX + 162, baseY + 280);
	putString("$s4:", baseX + 42, baseY + 300);
	putString("$s5:", baseX + 162, baseY + 300);
	putString("$s6:", baseX + 42, baseY + 320);
	putString("$s7:", baseX + 162, baseY + 320);
	putString("$t8:", baseX + 42, baseY + 340);
	putString("$t9:", baseX + 162, baseY + 340);
	putString("$k0:", baseX + 42, baseY + 360);
	putString("$k1:", baseX + 162, baseY + 360);
	putString("$gp:", baseX + 42, baseY + 380);
	putString("$sp:", baseX + 162, baseY + 380);
	putString("$fp:", baseX + 42, baseY + 400);
	putString("$ra:", baseX + 162, baseY + 400);
	for (int i = 0; i < 32; i++) {
		putQuad(baseX + 40 + (i & 1) * 120, baseY + 100 + i / 2 * 20,
			baseX + 40 + (i & 1) * 120 + 120, baseY + 120 + i / 2 * 20, EMPTY_FILL);
		sprintf(hex, "%08x", getReg(i));
		putString(hex, baseX + 92 + (i & 1) * 120, baseY + 100 + i / 2 * 20);
	}

	setColor(63, 63, 63);
	putQuad(baseX + 40, baseY + 440, baseX + sizeX - 40, baseY + sizeY - 40, EMPTY_FILL);
}
int MipsEmu::step() {
	long instruction = getMem(pc, 4);
	instNode tmp;
	tmp.value = instruction;
	switch (tmp["opcode"]) {
	case 0:
		switch (tmp["funct"]) {
		case 0x0:
			regs[tmp["rd"]] = regs[tmp["rt"]] <<tmp["shamt"];
			break;
		case 0x2:
			regs[tmp["rd"]] = regs[tmp["rt"]] >> tmp["shamt"];
			break;
		case 0x3:
			regs[tmp["rd"]] = (unsigned)regs[tmp["rt"]] >> tmp["shamt"];
			break;
		case 0x4:
			regs[tmp["rd"]] = regs[tmp["rt"]] << regs[tmp["rs"]];
			break;
		case 0x6:
			regs[tmp["rd"]] = regs[tmp["rt"]] >> regs[tmp["rs"]];
			break;
		case 0x7:
			regs[tmp["rd"]] = (unsigned)regs[tmp["rt"]] >> regs[tmp["rs"]];
			break;
		case 0x8:
			pc = regs[tmp["rs"]];
			break;
		case 0x9:
			pc = regs[tmp["rs"]];
			regs[tmp["rd"]] = pc + 4;
			break;
		case 0x10:
			regs[tmp["rd"]] = high;
			break;
		case 0x11:
			high = regs[tmp["rs"]];
			break;
		case 0x12:
			regs[tmp["rd"]] = low;
			break;
		case 0x13:
			low = regs[tmp["rs"]];
			break;
		case 0x18:
		case 0x19:
			low = (unsigned int)regs[tmp["rs"]] * (unsigned int)regs[tmp["rt"]];
			high = (long long)(((unsigned int)regs[tmp["rs"]] >> 16) * ((unsigned int)regs[tmp["rt"]] >> 16));
			break;
		case 0x1A:
		case 0x1B:
			low = regs[tmp["rs"]] / regs[tmp["rt"]];
			high = regs[tmp["rs"]] % regs[tmp["rt"]];
			break;
		case 0x20:
		case 0x21:
			regs[tmp["rd"]] = regs[tmp["rs"]] + regs[tmp["rt"]];
			break;
		case 0x22:
		case 0x23:
			regs[tmp["rd"]] = regs[tmp["rs"]] - regs[tmp["rt"]];
			break;
		case 0x24:
			regs[tmp["rd"]] = regs[tmp["rs"]] & regs[tmp["rt"]];
			break;
		case 0x25:
			regs[tmp["rd"]] = regs[tmp["rs"]] | regs[tmp["rt"]];
			break;
		case 0x26:
			regs[tmp["rd"]] = regs[tmp["rs"]] ^ regs[tmp["rt"]];
			break;
		case 0x27:
			regs[tmp["rd"]] = (regs[tmp["rs"]] && regs[tmp["rt"]]) ||
				(!regs[tmp["rs"]] && !regs[tmp["rt"]]);
			break;
		case 0x2a:
		case 0x2b:
			regs[tmp["rd"]] = regs[tmp["rs"]] < regs[tmp["rt"]];
			break;
		}
		pc += 4;
		break;
	case 1:
		if (tmp["rt"] == 0) {
			if (regs[tmp["rs"]] < 0)pc += (short)tmp["imm"];
			else pc += 4;
		}
		else {
			if (regs[tmp["rs"]] >= 0)pc += (short)tmp["imm"];
			else pc += 4;
		}
		break;
	case 2:
		pc = (pc & 0xF0000000 ) + tmp["target"] * 4;
		break;
	case 3:
		pc = (pc & 0xF0000000) + tmp["target"] * 4;
		regs[31] = pc + 4;
		break;
	case 4:
		if (regs[tmp["rs"]] == regs[tmp["rt"]])pc += (short)tmp["imm"];
		else pc += 4;
		break;
	case 5:
		if (regs[tmp["rs"]] != regs[tmp["rt"]])pc += (short)tmp["imm"];
		else pc += 4;
		break;
	case 6:
		if (regs[tmp["rs"]] <= 0)pc += (short)tmp["imm"];
		else pc += 4;
		break;
	case 7:
		if (regs[tmp["rs"]] > 0)pc += (short)tmp["imm"];
		else pc += 4;
		break;
	case 8:
	case 9:
		regs[tmp["rt"]] = regs[tmp["rs"]] + (short)tmp["imm"];
		pc += 4;
		break;
	case 10:
	case 11:
		regs[tmp["rt"]] = regs[tmp["rs"]] < (short)tmp["imm"];
		pc += 4;
		break;
	case 12:
		regs[tmp["rt"]] = regs[tmp["rs"]] & (short)tmp["imm"];
		pc += 4;
		break;
	case 13:
		regs[tmp["rt"]] = regs[tmp["rs"]] | (short)tmp["imm"];
		pc += 4;
		break;
	case 14:
		regs[tmp["rt"]] = regs[tmp["rs"]] ^ (short)tmp["imm"];
		pc += 4;
		break;
	case 15:
		regs[tmp["rt"]] &= 0xFFFF;
		regs[tmp["rt"]] |= (tmp["imm"] & 0xFFFF) << 16;
		pc += 4;
		break;
	case 32:
		regs[tmp["rt"]] = *(char *)(mem + (short)tmp["imm"]);
		pc += 4;
		break;
	case 33:
		regs[tmp["rt"]] = *(short *)(mem + (short)tmp["imm"]);
		pc += 4;
		break;
	case 35:
		regs[tmp["rt"]] = *(long *)(mem + (short)tmp["imm"]);
		pc += 4;
		break;
	case 36:
		regs[tmp["rt"]] = *(unsigned char *)(mem + (short)tmp["imm"]);
		pc += 4;
		break;
	case 37:
		regs[tmp["rt"]] = *(unsigned short *)(mem + (short)tmp["imm"]);
		pc += 4;
		break;
	case 40:
		*(char *)(mem + (short)tmp["imm"]) = (char)regs[tmp["rt"]];
		pc += 4;
		break;
	case 41:
		*(short *)(mem + (short)tmp["imm"]) = (short)regs[tmp["rt"]];
		pc += 4;
		break;
	case 43:
		*(long *)(mem + (short)tmp["imm"]) = regs[tmp["rt"]];
		pc += 4;
		break;
	}

	for (unsigned int i = 0; i < output.size(); i++) {
		if (output[i].offset == pc)return output[i].line;
	}
	return 0;
}
int MipsEmu::process() {
	return 0;
}
void MipsEmu::stop() {

}
