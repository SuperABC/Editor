#define _CRT_SECURE_NO_WARNINGS
#include "machine.h"
#include "../editor/editor.h"
#include <time.h>

extern Editor *editor;

Machine::Machine() {

}
Machine::~Machine() {
	
}

Machine *Machine::input(stateSeq *seq,
	vector<classType> &classType, vector<funcNode> &func,
	vector<varNode> &var) {
	content = seq;
	globeClassType = classType;
	globeFunc = func;

	for (unsigned int i = globeVar.size(); i < var.size(); i++) {
		globeVar.push_back(var[i]);
	}
	return this;
}
void Machine::execute() {
	stateSeq *iter = content;
	while (iter) {
		step(iter);
		iter = iter->next;
	}
}
void Machine::execute(stateSeq *s, varNode *par, int funcid) {
	int preFunc = func;
	if (funcid != -1)func = funcid;
	stateSeq *iter = s;

	while (par != NULL) {
		for (auto &v : globeFunc[func].localVar) {
			if (v.name == par->name) {
				v.t = par->t;
				v.val = par->val;
			}
		}

		par = par->next;
	}
	while (iter) {
		step(iter);
		iter = iter->next;
	}

	func = preFunc;
}
void Machine::step(stateSeq *s) {
	if (s->act.op == VO_ASSIGN) {
		int offset = -1;
		string element = string();
		if (s->act.left->name[0] >= '0' && s->act.left->name[0] <= '9') {
			offset = atoi(s->act.left->name.c_str());
			s->act.left->name = s->act.left->name.c_str() + s->act.left->name.find('@') + 1;
		}
		else if ((offset = s->act.left->name.find('@')) != -1) {
			element = s->act.left->name.substr(0, offset);
		}
		for (unsigned int i = 0; i < globeVar.size(); i++) {
			if (globeVar[i].name == s->act.left->name) {
				varNode tmp = exp(s->act.right);
				switch (s->act.left->t) {
				case _VT_INTEGER:
					if (tmp.t == _VT_INTEGER) {
						globeVar[i].val = tmp.val;
					}
					else if (tmp.t == _VT_FLOAT) {
						globeVar[i].val = new int((int)*(float *)tmp.val);
					}
					else if (tmp.t == _VT_BOOL) {
						globeVar[i].val = new int(*(bool *)tmp.val);
					}
					else if (tmp.t == _VT_CHAR) {
						globeVar[i].val = new int(*(char *)tmp.val);
					}
					else
						error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

					break;
				case _VT_BOOL:
					if (tmp.t == _VT_INTEGER) {
						globeVar[i].val = new bool(*(int *)tmp.val);
					}
					else if (tmp.t == _VT_FLOAT) {
						globeVar[i].val = new bool(*(float *)tmp.val);
					}
					else if (tmp.t == _VT_BOOL) {
						globeVar[i].val = tmp.val;
					}
					else if (tmp.t == _VT_CHAR) {
						globeVar[i].val = new bool(*(char *)tmp.val);
					}
					else
						error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

					break;
				case _VT_CHAR:
					if (tmp.t == _VT_INTEGER) {
						globeVar[i].val = new char(*(int *)tmp.val);
					}
					else if (tmp.t == _VT_FLOAT) {
						globeVar[i].val = new char((char)*(float *)tmp.val);
					}
					else if (tmp.t == _VT_BOOL) {
						globeVar[i].val = new char(*(bool *)tmp.val);
					}
					else if (tmp.t == _VT_CHAR) {
						globeVar[i].val = tmp.val;
					}
					else
						error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

					break;
				case _VT_FLOAT:
					if (tmp.t == _VT_INTEGER) {
						globeVar[i].val = new float((float)*(int *)tmp.val);
					}
					else if (tmp.t == _VT_FLOAT) {
						globeVar[i].val = tmp.val;
					}
					else if (tmp.t == _VT_BOOL) {
						globeVar[i].val = new float(*(bool *)tmp.val);
					}
					else if (tmp.t == _VT_CHAR) {
						globeVar[i].val = new float(*(char *)tmp.val);
					}
					else
						error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

					break;
				case _VT_STRING:
					if (tmp.t == _VT_STRING)
						globeVar[i].val = tmp.val;
					else
						error(s->act.left->name.c_str(), VE_TYPEMISMATCH);
					break;
				case _VT_ARRAY:
					if (offset == -1) {
						if (tmp.t == _VT_ARRAY)
							globeVar[i].val = tmp.val;
						else
							error(s->act.left->name.c_str(), VE_TYPEMISMATCH);
					}
					else {
						varNode *start = (varNode *)globeVar[i].val;
						while (offset--) {
							start = start->next;
						}
						switch (start->t) {
						case _VT_INTEGER:
							if (tmp.t == _VT_INTEGER) {
								start->val = tmp.val;
							}
							else if (tmp.t == _VT_FLOAT) {
								start->val = new int((int)*(float *)tmp.val);
							}
							else if (tmp.t == _VT_BOOL) {
								start->val = new int(*(bool *)tmp.val);
							}
							else if (tmp.t == _VT_CHAR) {
								start->val = new int(*(char *)tmp.val);
							}
							else
								error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

							break;
						case _VT_BOOL:
							if (tmp.t == _VT_INTEGER) {
								start->val = new bool(*(int *)tmp.val);
							}
							else if (tmp.t == _VT_FLOAT) {
								start->val = new bool(*(float *)tmp.val);
							}
							else if (tmp.t == _VT_BOOL) {
								start->val = tmp.val;
							}
							else if (tmp.t == _VT_CHAR) {
								start->val = new bool(*(char *)tmp.val);
							}
							else
								error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

							break;
						case _VT_CHAR:
							if (tmp.t == _VT_INTEGER) {
								start->val = new char(*(int *)tmp.val);
							}
							else if (tmp.t == _VT_FLOAT) {
								start->val = new char((char)*(float *)tmp.val);
							}
							else if (tmp.t == _VT_BOOL) {
								start->val = new char(*(bool *)tmp.val);
							}
							else if (tmp.t == _VT_CHAR) {
								start->val = tmp.val;
							}
							else
								error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

							break;
						case _VT_FLOAT:
							if (tmp.t == _VT_INTEGER) {
								start->val = new float((float)*(int *)tmp.val);
							}
							else if (tmp.t == _VT_FLOAT) {
								start->val = tmp.val;
							}
							else if (tmp.t == _VT_BOOL) {
								start->val = new float(*(bool *)tmp.val);
							}
							else if (tmp.t == _VT_CHAR) {
								start->val = new float(*(char *)tmp.val);
							}
							else
								error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

							break;
						case _VT_STRING:
							if (tmp.t == _VT_STRING)
								start->val = tmp.val;
							else
								error(s->act.left->name.c_str(), VE_TYPEMISMATCH);
							break;
						}
					}

					break;
				case _VT_CLASS:
					if (tmp.t == _VT_CLASS && tmp.classType == s->act.left->classType) {
						if (element.empty()) {
							globeVar[i].val = tmp.val;
						}
						else {
							for (auto &ele : ((classNode *)globeVar[i].val)->varList) {
								if (ele.name == element) {
									ele.val = tmp.val;
									break;
								}
							}
						}
					}
					else
						error(s->act.left->classType.c_str(), VE_TYPEMISMATCH);
					break;
				case _VT_NULL:
					globeVar[i].t = tmp.t;
					globeVar[i].val = tmp.val;
					globeVar[i].classType = tmp.classType;

					break;
				}

				break;
			}
		}
		if (func != -1) {
			for (unsigned int i = 0; i < globeFunc[func].localVar.size(); i++) {
				if (globeFunc[func].localVar[i].name == s->act.left->name) {
					varNode tmp = exp(s->act.right);
					switch (s->act.left->t) {
					case _VT_INTEGER:
						if (tmp.t == _VT_INTEGER) {
							globeFunc[func].localVar[i].val = tmp.val;
						}
						else if (tmp.t == _VT_FLOAT) {
							globeFunc[func].localVar[i].val = new int((int)*(float *)tmp.val);
						}
						else if (tmp.t == _VT_BOOL) {
							globeFunc[func].localVar[i].val = new int(*(bool *)tmp.val);
						}
						else if (tmp.t == _VT_CHAR) {
							globeFunc[func].localVar[i].val = new int(*(char *)tmp.val);
						}
						else
							error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

						break;
					case _VT_BOOL:
						if (tmp.t == _VT_INTEGER) {
							globeFunc[func].localVar[i].val = new bool(*(int *)tmp.val);
						}
						else if (tmp.t == _VT_FLOAT) {
							globeFunc[func].localVar[i].val = new bool(*(float *)tmp.val);
						}
						else if (tmp.t == _VT_BOOL) {
							globeFunc[func].localVar[i].val = tmp.val;
						}
						else if (tmp.t == _VT_CHAR) {
							globeFunc[func].localVar[i].val = new bool(*(char *)tmp.val);
						}
						else
							error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

						break;
					case _VT_CHAR:
						if (tmp.t == _VT_INTEGER) {
							globeFunc[func].localVar[i].val = new char(*(int *)tmp.val);
						}
						else if (tmp.t == _VT_FLOAT) {
							globeFunc[func].localVar[i].val = new char((char)*(float *)tmp.val);
						}
						else if (tmp.t == _VT_BOOL) {
							globeFunc[func].localVar[i].val = new char(*(bool *)tmp.val);
						}
						else if (tmp.t == _VT_CHAR) {
							globeFunc[func].localVar[i].val = tmp.val;
						}
						else
							error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

						break;
					case _VT_FLOAT:
						if (tmp.t == _VT_INTEGER) {
							globeFunc[func].localVar[i].val = new float((float)*(int *)tmp.val);
						}
						else if (tmp.t == _VT_FLOAT) {
							globeFunc[func].localVar[i].val = tmp.val;
						}
						else if (tmp.t == _VT_BOOL) {
							globeFunc[func].localVar[i].val = new float(*(bool *)tmp.val);
						}
						else if (tmp.t == _VT_CHAR) {
							globeFunc[func].localVar[i].val = new float(*(char *)tmp.val);
						}
						else
							error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

						break;
					case _VT_STRING:
						if (tmp.t == _VT_STRING)
							globeFunc[func].localVar[i].val = tmp.val;
						else
							error(s->act.left->name.c_str(), VE_TYPEMISMATCH);
						break;
					case _VT_ARRAY:
						if (offset == -1) {
							if (tmp.t == _VT_ARRAY)
								globeVar[i].val = tmp.val;
							else
								error(s->act.left->name.c_str(), VE_TYPEMISMATCH);
						}
						else {
							varNode *start = (varNode *)globeFunc[func].localVar[i].val;
							while (offset--) {
								start = start->next;
							}
							switch (start->t) {
							case _VT_INTEGER:
								if (tmp.t == _VT_INTEGER) {
									start->val = tmp.val;
								}
								else if (tmp.t == _VT_FLOAT) {
									start->val = new int((int)*(float *)tmp.val);
								}
								else if (tmp.t == _VT_BOOL) {
									start->val = new int(*(bool *)tmp.val);
								}
								else if (tmp.t == _VT_CHAR) {
									start->val = new int(*(char *)tmp.val);
								}
								else
									error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

								break;
							case _VT_BOOL:
								if (tmp.t == _VT_INTEGER) {
									start->val = new bool(*(int *)tmp.val);
								}
								else if (tmp.t == _VT_FLOAT) {
									start->val = new bool(*(float *)tmp.val);
								}
								else if (tmp.t == _VT_BOOL) {
									start->val = tmp.val;
								}
								else if (tmp.t == _VT_CHAR) {
									start->val = new bool(*(char *)tmp.val);
								}
								else
									error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

								break;
							case _VT_CHAR:
								if (tmp.t == _VT_INTEGER) {
									start->val = new char(*(int *)tmp.val);
								}
								else if (tmp.t == _VT_FLOAT) {
									start->val = new char((char)*(float *)tmp.val);
								}
								else if (tmp.t == _VT_BOOL) {
									start->val = new char(*(bool *)tmp.val);
								}
								else if (tmp.t == _VT_CHAR) {
									start->val = tmp.val;
								}
								else
									error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

								break;
							case _VT_FLOAT:
								if (tmp.t == _VT_INTEGER) {
									start->val = new float((float)*(int *)tmp.val);
								}
								else if (tmp.t == _VT_FLOAT) {
									start->val = tmp.val;
								}
								else if (tmp.t == _VT_BOOL) {
									start->val = new float(*(bool *)tmp.val);
								}
								else if (tmp.t == _VT_CHAR) {
									start->val = new float(*(char *)tmp.val);
								}
								else
									error(s->act.left->name.c_str(), VE_TYPEMISMATCH);

								break;
							case _VT_STRING:
								if (tmp.t == _VT_STRING)
									start->val = tmp.val;
								else
									error(s->act.left->name.c_str(), VE_TYPEMISMATCH);
								break;
							}
						}

						break;
					case _VT_CLASS:
						if (tmp.t == _VT_CLASS && tmp.classType == s->act.left->classType) {
							if (element.empty()) {
								globeFunc[func].localVar[i].val = tmp.val;
							}
							else {
								for (auto &ele : ((classNode *)globeFunc[func].localVar[i].val)->varList) {
									if (ele.name == element) {
										ele.val = tmp.val;
										break;
									}
								}
							}
						}
						else
							error(s->act.left->classType.c_str(), VE_TYPEMISMATCH);
						break;
					case _VT_NULL:
						globeFunc[func].localVar[i].t = tmp.t;
						globeFunc[func].localVar[i].val = tmp.val;
						globeFunc[func].localVar[i].classType = tmp.classType;

						break;
					}

					break;
				}
			}
		}
	}
	else if (s->act.op == VO_EXE) {
		varNode *iter = s->act.right;
		varNode *par = new varNode();
		varNode *start = par;
		while (iter != NULL) {
			*par = exp(iter);
			par->name = iter->name;
			iter = iter->next;
			if(iter)par->next = new varNode();
			par = par->next;
		}
		if (s->act.left->name == "out") {
			out(start);
		}
		else if (s->act.left->name == "outln") {
			outln(start);
		}
		else if (s->act.left->name == "current time") {
			time(start);
		}
		else if(s->act.left->name == "in") {
			in(start);
		}
		else {
			for (unsigned int i = 0; i < globeFunc.size(); i++) {
				if (globeFunc[i].declare.name == s->act.left->name) {
					execute(&globeFunc[i].content, start, i);
				}
			}
		}
	}
	else if (s->act.op == VO_IF) {
		varNode res = exp(s->act.left);
		bool ar;
		switch (res.t) {
		case _VT_INTEGER:
			if (*(int *)res.val)ar = true;
			else ar = false;
			break;
		case _VT_BOOL:
			ar = *(bool *)res.val;
			break;
		}
		if (ar) {
			execute(s->act.right->left->block, NULL);
		}
		else {
			execute(s->act.right->right->block, NULL);
		}
	}
	else if (s->act.op == VO_WHILE) {
		while (true) {
			varNode cond = exp(s->act.left);

			bool loop;
			switch (cond.t) {
			case _VT_INTEGER:
				if (*(int *)cond.val)loop = true;
				else loop = false;
				break;
			case _VT_BOOL:
				loop = *(bool *)cond.val;
				break;
			}
			if (!loop)break;

			execute(s->act.right->block, NULL);
		}
	}
}
varNode Machine::exp(varNode *e) {
	varNode ret;
	if (e->t == _VT_FUNCTION) {
		if (e->block->act.left->name == "current time") {
			ret = time(e->block->act.right);
		}
		else if (e->block->act.left->name == "in") {
			ret = in(e->block->act.right);
		}
		else if (e->block->act.left->name == "random number") {
			ret = random(e->block->act.right);
		}
		else {
			for (unsigned int i = 0; i < globeFunc.size(); i++) {
				if (globeFunc[i].declare.name == e->block->act.left->name) {
					execute(&globeFunc[i].content, e->block->act.right, i);
					ret = globeFunc[i].localVar[0];
				}
			}
		}
	}
	else if (e->t == _VT_EXP) {
		varNode l = exp(e->left);
		varNode r = exp(e->right);

		switch (*(int *)e->val) {
		case OP_PLUS:
			if (l.t == _VT_FLOAT && r.t == _VT_FLOAT) {
				ret.t = _VT_FLOAT;
				ret.val = new float(*(float *)(l.val) + *(float *)(r.val));
			}
			else if (l.t == _VT_FLOAT && r.t == _VT_INTEGER) {
				ret.t = _VT_FLOAT;
				ret.val = new float(*(float *)(l.val) + *(int *)(r.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_FLOAT) {
				ret.t = _VT_FLOAT;
				ret.val = new float(*(int *)(l.val) + *(float *)(r.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_INTEGER) {
				ret.t = _VT_INTEGER;
				ret.val = new int(*(int *)(l.val) + *(int *)(r.val));
			}
			else if (l.t == _VT_STRING && r.t == _VT_STRING) {
				ret.t = _VT_STRING;
				ret.val = new char[strlen((char *)l.val) + strlen((char *)r.val)];
				strcpy((char *)ret.val, (char *)r.val);
				strcat((char *)ret.val, (char *)l.val);
			}
			break;
		case OP_MINUS:
			if (l.t == _VT_FLOAT && r.t == _VT_FLOAT) {
				ret.t = _VT_FLOAT;
				ret.val = new float(*(float *)(r.val) - *(float *)(l.val));
			}
			else if (l.t == _VT_FLOAT && r.t == _VT_INTEGER) {
				ret.t = _VT_FLOAT;
				ret.val = new float(*(int *)(r.val) - *(float *)(l.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_FLOAT) {
				ret.t = _VT_FLOAT;
				ret.val = new float(*(float *)(r.val) - *(int *)(l.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_INTEGER) {
				ret.t = _VT_INTEGER;
				ret.val = new int(*(int *)(r.val) - *(int *)(l.val));
			}
			break;
		case OP_MULTY:
			if (l.t == _VT_FLOAT && r.t == _VT_FLOAT) {
				ret.t = _VT_FLOAT;
				ret.val = new float(*(float *)(l.val) * *(float *)(r.val));
			}
			else if (l.t == _VT_FLOAT && r.t == _VT_INTEGER) {
				ret.t = _VT_FLOAT;
				ret.val = new float(*(float *)(l.val) * *(int *)(r.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_FLOAT) {
				ret.t = _VT_FLOAT;
				ret.val = new float(*(int *)(l.val) * *(float *)(r.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_INTEGER) {
				ret.t = _VT_INTEGER;
				ret.val = new int(*(int *)(l.val) * *(int *)(r.val));
			}
			break;
		case OP_DIVIDE:
			if (l.t == _VT_FLOAT && r.t == _VT_FLOAT) {
				ret.t = _VT_FLOAT;
				if (*(float *)(l.val) == 0)
					error("", VE_DIVBYZERO);
				ret.val = new float(*(float *)(r.val) / *(float *)(l.val));
			}
			else if (l.t == _VT_FLOAT && r.t == _VT_INTEGER) {
				ret.t = _VT_FLOAT;
				if (*(float *)(l.val) == 0)
					error("", VE_DIVBYZERO);
				ret.val = new float(*(int *)(r.val) / *(float *)(l.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_FLOAT) {
				ret.t = _VT_FLOAT;
				if (*(int *)(l.val) == 0)
					error("", VE_DIVBYZERO);
				ret.val = new float(*(float *)(r.val) / *(int *)(l.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_INTEGER) {
				ret.t = _VT_INTEGER;
				if (*(int *)(l.val) == 0)
					error("", VE_DIVBYZERO);
				ret.val = new int(*(int *)(r.val) / *(int *)(l.val));
			}
			break;
		case OP_MOD:
			if (l.t == _VT_INTEGER && r.t == _VT_INTEGER) {
				ret.t = _VT_INTEGER;
				ret.val = new int(*(int *)(r.val) % *(int *)(l.val));
			}
			else error("mod", VE_TYPEMISMATCH);
			break;
		case OP_EQUAL:
			if (l.t == _VT_FLOAT && r.t == _VT_FLOAT) {
				ret.t = _VT_BOOL;
ret.val = new bool(*(float *)(r.val) == *(float *)(l.val));
			}
			else if (l.t == _VT_FLOAT && r.t == _VT_INTEGER) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(int *)(r.val) == *(float *)(l.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_FLOAT) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(float *)(r.val) == *(int *)(l.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_INTEGER) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(int *)(r.val) == *(int *)(l.val));
			}
			break;
		case OP_GREATER:
			if (l.t == _VT_FLOAT && r.t == _VT_FLOAT) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(float *)(r.val) > *(float *)(l.val));
			}
			else if (l.t == _VT_FLOAT && r.t == _VT_INTEGER) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(int *)(r.val) > *(float *)(l.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_FLOAT) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(float *)(r.val) > *(int *)(l.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_INTEGER) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(int *)(r.val) > *(int *)(l.val));
			}
			break;
		case OP_SMALLER:
			if (l.t == _VT_FLOAT && r.t == _VT_FLOAT) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(float *)(r.val) < *(float *)(l.val));
			}
			else if (l.t == _VT_FLOAT && r.t == _VT_INTEGER) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(int *)(r.val) < *(float *)(l.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_FLOAT) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(float *)(r.val) < *(int *)(l.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_INTEGER) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(int *)(r.val) < *(int *)(l.val));
			}
			break;
		case OP_NSMALLER:
			if (l.t == _VT_FLOAT && r.t == _VT_FLOAT) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(float *)(r.val) >= *(float *)(l.val));
			}
			else if (l.t == _VT_FLOAT && r.t == _VT_INTEGER) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(int *)(r.val) >= *(float *)(l.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_FLOAT) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(float *)(r.val) >= *(int *)(l.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_INTEGER) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(int *)(r.val) >= *(int *)(l.val));
			}
			break;
		case OP_NGREATER:
			if (l.t == _VT_FLOAT && r.t == _VT_FLOAT) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(float *)(r.val) <= *(float *)(l.val));
			}
			else if (l.t == _VT_FLOAT && r.t == _VT_INTEGER) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(int *)(r.val) <= *(float *)(l.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_FLOAT) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(float *)(r.val) <= *(int *)(l.val));
			}
			else if (l.t == _VT_INTEGER && r.t == _VT_INTEGER) {
				ret.t = _VT_BOOL;
				ret.val = new bool(*(int *)(r.val) <= *(int *)(l.val));
			}
			break;
		}
	}
	else if (e->t == _VT_VAR) {
		int offset = -1;
		string element = string();
		if (((char *)e->val)[0] >= '0' && ((char *)e->val)[0] <= '9') {
			offset = atoi((char *)e->val);
			while (*(char *)e->val != '@') {
				e->val = (char *)e->val + 1;
			}
			e->val = (char *)e->val + 1;
		}
		else if ((offset = string((char *)e->val).find('@'))!=-1) {
			element = string((char *)e->val).substr(0, offset);
			e->val = (char *)e->val + offset + 1;
		}
		for (auto v : globeVar) {
			if ((char *)e->val == v.name) {
				if (offset == -1) {
					ret.t = v.t;
					ret.val = v.val;
					break;
				}
				else if(element.empty()) {
					varNode *start = (varNode *)v.val;
					while (offset--) {
						start = start->next;
					}
					ret.t = start->t;
					ret.val = start->val;
					break;
				}
				else {
					classNode *parent = (classNode *)v.val;
					for(unsigned int i = 0; i < parent->type.varList.size(); i++) {
						if (parent->type.varList[i].name == element) {
							ret.t = parent->varList[i].t;
							ret.val = parent->varList[i].val;
							break;
						}
					}
				}
			}
		}
		if (func != -1) {
			for (auto v : globeFunc[func].localVar) {
				if ((char *)e->val == v.name) {
					ret.t = v.t;
					ret.val = v.val;
					break;
				}
			}
		}
	}
	else {
		ret.t = e->t;
		ret.classType = e->classType;
		ret.val = e->val;
		ret.next = e->next;
	}

	return ret;
}

void Machine::print(varNode *par) {
	char tmp[64] = "";

	switch (par->t) {
	case _VT_STRING: {
		sprintf(tmp, "%s", (char *)par->val);
		break;
	}
	case _VT_INTEGER: {
		sprintf(tmp, "%d", *(int *)(par->val));
		break;
	}
	case _VT_FLOAT: {
		sprintf(tmp, "%f", *(float *)(par->val));
		break;
	}
	case _VT_CHAR: {
		sprintf(tmp, "%c", *(char *)(par->val));
		break;
	}
	case _VT_BOOL: {
		if(*(bool *)(par->val))
			sprintf(tmp, "true");
		else sprintf(tmp, "false");
		break;
	}
	case _VT_VAR: {
		print(&exp(par));
		break;
	}
	case _VT_ARRAY: {
		varNode * iter = (varNode *)par->val;
		while (iter) {
			print(iter);
			editor->logPrint(" ");
			iter = iter->next;
		}
		break;
	}
	case _VT_CLASS: {
		classNode * tmp = (classNode *)par->val;
		editor->logPrint("{ ");
		for (auto &iter : tmp->varList) {
			print(&iter);
			editor->logPrint(" ");
		}
		editor->logPrint("}");
		break;
	}

	}
	editor->logPrint(tmp);
}
void Machine::out(varNode *par) {
	if (par->name == "content") {
		print(par);
	}
}
void Machine::outln(varNode *par) {
	if (par->name == "content") {
		print(par);
		editor->logPrint("\n");
	}
}
varNode Machine::in(varNode *par) {
	for (auto &f : globeFunc) {
		if (f.declare.name == "in") {
			string *tmp = new string();
			std::cin >> *tmp;

			if ((*tmp)[0] >= '0'&&(*tmp)[0] <= '9') {
				if (tmp->find('.') == -1) {
					f.localVar[0].t = _VT_INTEGER;
					f.localVar[0].val = new int(atoi(tmp->c_str()));
					return f.localVar[0];
				}
				else {
					f.localVar[0].t = _VT_FLOAT;
					f.localVar[0].val = new float((float)atof(tmp->c_str()));
					return f.localVar[0];
				}
			}
			else {
				f.localVar[0].t = _VT_STRING;
				f.localVar[0].val = tmp->c_str();
				return f.localVar[0];
			}

		}
	}
	error("", VE_BROKEN);
	return varNode();
}
varNode Machine::time(varNode *par) {
	for (auto &f : globeFunc) {
		if (f.declare.name == "current time") {
			f.localVar[0].t = _VT_INTEGER;
			f.localVar[0].val = new int(clock());
			return f.localVar[0];
		}
	}
	error("", VE_BROKEN);
	return varNode();
}
varNode Machine::random(varNode *par) {
	for (auto &f : globeFunc) {
		if (f.declare.name == "random number") {
			f.localVar[0].t = _VT_INTEGER;
			f.localVar[0].val = new int(rand());
			return f.localVar[0];
		}
	}
	error("", VE_BROKEN);
	return varNode();
}

void Machine::clearMem() {
	macMem.clear();
}

void Machine::error(const char *inst, int type) {
	switch (type) {
	case VE_DIVBYZERO:
		throw new MachineException(std::string("0��������������"));
	case VE_TYPEMISMATCH:
		throw new MachineException(std::string(inst) + "���Ͳ�ƥ�䡣");
	case VE_BROKEN:
		throw new MachineException("�����δ׼�������ѱ��ƻ���");
	}
}