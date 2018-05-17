#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include "ILemitter.h"
#include "AstDefs.h"

struct AstNode {
    AstNodeType node_type;
    unsigned int line, column;
    bool emit = true;

    std::vector<AstAttribute *> attributes;

    AstNode(AstNodeType node_type, unsigned int line, unsigned int column) : node_type(node_type), line(line), column(column) {}

    virtual ~AstNode() {}
};

struct AstString : public AstNode {
    std::string value;

    AstString(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstString, line, column) {}
};

struct AstNumber : public AstNode {
    bool is_float  = false;
    bool is_signed = true;
    int bits;

    union {
        uint64_t u;
        int64_t i;
        double f;
    } value;

    AstNumber(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstNumber, line, column) {}
};

struct AstBoolean : public AstNode {
    bool value;

    AstBoolean(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstBoolean, line, column) {}
};

struct AstType : public AstNode {
    std::string name;
    bool is_array = false;
    AstType *subtype = nullptr;

    AstType(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstType, line, column) {}

    virtual ~AstType() {
        delete subtype;
    }
};

struct AstArray : public AstNode {
    std::vector<AstNode *> elements;
    AstType *ele_type = nullptr;

    AstArray(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstArray, line, column) {}

    virtual ~AstArray() {
        delete ele_type;
        for (auto p : elements) {
            delete p;
        }
    }
};

struct AstSymbol : public AstNode {
    std::string name;

    AstSymbol(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstSymbol, line, column) {}
};

struct AstBlock : public AstNode {
    std::vector<AstNode *> statements;

    AstBlock(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstBlock, line, column) {}

    virtual ~AstBlock() {
        for (auto *p : statements) {
            delete p;
        }
    }
};

struct AstDec : public AstNode {
    std::string name;
    AstType *type  = nullptr;
    AstNode *value = nullptr;
    bool immutable = false;

    AstDec(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstDec, line, column) {}

    virtual ~AstDec() {
        delete type;
        delete value;
    }
};

struct AstIf : public AstNode {
    AstNode *condition = nullptr;
    AstBlock *true_block = nullptr, *false_block = nullptr;

    AstIf(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstIf, line, column) {}

    virtual ~AstIf() {
        delete condition;
        delete true_block;
        delete false_block;
    }
};

struct AstFn : public AstNode {
    std::string unmangled_name;
    std::string mangled_name;
    std::string type_self;
    std::vector<AstDec *> params;
    AstType *return_type = nullptr;
    AstBlock *body = nullptr;

    AstFn(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstFn, line, column) {}

    virtual ~AstFn() {
        delete return_type;
        delete body;

        for (auto *p : params) {
            delete p;
        }
    }
};

struct AstFnCall : public AstNode {
    std::string name;
    std::vector<AstNode *> args;
    bool mangled = false;

    AstFnCall(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstFnCall, line, column) {}

    virtual ~AstFnCall() {
        for (auto *p : args) {
            delete p;
        }
    }
};

struct AstLoop : public AstNode {
    std::string name;
    bool is_foreach = false;
    AstBlock *body = nullptr;
    AstNode  *expr = nullptr;

    AstLoop(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstLoop, line, column) {}

    virtual ~AstLoop() {
        delete body;
        delete expr;
    }
};

struct AstContinue : public AstNode {
    AstContinue(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstContinue, line, column) {}
};

struct AstBreak : public AstNode {
    AstBreak(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstBreak, line, column) {}
};

struct AstStruct : public AstNode {
    std::string name;
    AstBlock *block = nullptr;

    AstStruct(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstStruct, line, column) {}

    virtual ~AstStruct() {
        delete block;
    }
};

struct AstImpl : public AstNode {
    std::string name;
    AstBlock *block = nullptr;

    AstImpl(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstImpl, line, column) {}

    virtual ~AstImpl() {
        delete block;
    }
};

struct AstAttribute : public AstNode {
    std::string name;
    std::vector<AstNode *> args;

    AstAttribute(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstAttribute, line, column) {}

    virtual ~AstAttribute() {
        for (auto *p : args) {
            delete p;
        }
    }
};

struct AstAffix : public AstNode {
    std::string unmangled_name;
    std::string mangled_name;
    std::vector<AstDec *> params;
    AstType *return_type = nullptr;
    AstBlock *body = nullptr;
    AffixType affix_type;
    bool mangled = false;

    AstAffix(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstAffix, line, column) {}

    virtual ~AstAffix() {
        delete return_type;
        delete body;

        for (auto *p : params) {
            delete p;
        }
    }
};

struct AstReturn : public AstNode {
    AstNode *expr = nullptr;

    AstReturn(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstReturn, line, column) {}

    virtual ~AstReturn() {
        delete expr;
    }
};

struct AstUnaryExpr : public AstNode {
    std::string op;
    AstNode *expr = nullptr;

    AstUnaryExpr(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstUnaryExpr, line, column) {}

    virtual ~AstUnaryExpr() {
        delete expr;
    }
};

struct AstBinaryExpr : public AstNode {
    std::string op;
    AstNode *lhs = nullptr, *rhs = nullptr;
    bool mangled = false;

    AstBinaryExpr(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstBinaryExpr, line, column) {}

    virtual ~AstBinaryExpr() {
        delete lhs;
        delete rhs;
    }
};

struct AstIndex : public AstNode {
    AstNode *array = nullptr, *expr = nullptr;

    AstIndex(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstIndex, line, column) {}

    virtual ~AstIndex() {
        delete array;
        delete expr;
    }
};

struct AstExtern : public AstNode {
    std::vector<AstFn *> decls;

    AstExtern(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstExtern, line, column) {}

    virtual ~AstExtern() {
        for (auto *p : decls) {
            delete p;
        }
    }
};

struct AstUse : public AstNode {
    std::string name;

    AstUse(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstUse, line, column) {}
};

struct AstNamespace : public AstNode {
    std::string name;

    AstNamespace(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstNamespace, line, column) {}
};

#endif /* AST_H */
