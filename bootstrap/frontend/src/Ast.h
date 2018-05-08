#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

#define AstNodeType_ENUM(name) name
#define AstNodeType_NAME_ARRAY(name) #name

#define AstNodeTypes(F)                                                        \
    F(AstBlock), F(AstString), F(AstNumber), F(AstBoolean), F(AstArray),       \
    F(AstDec), F(AstIf), F(AstFn), F(AstFnCall), F(AstLoop),               \
    F(AstContinue), F(AstBreak), F(AstStruct), F(AstImpl),                 \
    F(AstAttribute), F(AstAffix), F(AstUnaryExpr), F(AstBinaryExpr),       \
    F(AstIndex), F(AstType), F(AstSymbol), F(AstReturn), F(AstExtern),

enum class AstNodeType { AstNodeTypes(AstNodeType_ENUM) };
static constexpr const char *const ast_node_type_names[] = {
    AstNodeTypes(AstNodeType_NAME_ARRAY)
};

enum class AffixType {
    Infix,
    Prefix,
    Suffix,
};

typedef struct AstAttribute AstAttribute;

struct AstNode {
    AstNodeType node_type;
    unsigned int line, column;
    bool emit = true;

    std::vector<AstAttribute *> nested_attributes;

    AstNode(AstNodeType node_type, unsigned int line, unsigned int column):
        node_type(node_type), line(line), column(column) {}

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

struct AstArray : public AstNode {
    std::vector<AstNode *> elements;

    AstArray(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstArray, line, column) {}

    virtual ~AstArray() {
        for(auto p : elements) {
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
        for(auto *p : statements) {
            delete p;
        }
    }
};

struct AstType : public AstNode {
    std::string name;
    bool is_array    = false;
    AstType *subtype = nullptr;

    AstType(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstType, line, column) {}

    virtual ~AstType() {
        if(subtype) {
            delete subtype;
        }
    }
};

struct AstDec : public AstNode {
    AstSymbol *name = nullptr;
    AstType *type = nullptr;
    AstNode *value  = nullptr;
    bool immutable  = false;

    AstDec(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstDec, line, column) {}

    virtual ~AstDec() {
        if(name) {
            delete name;
        }

        if(type) {
            delete type;
        }

        if(value) {
            delete value;
        }
    }
};

struct AstIf : public AstNode {
    AstNode *condition   = nullptr;
    AstBlock *true_block = nullptr, *false_block = nullptr;

    AstIf(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstIf, line, column) {}

    virtual ~AstIf() {
        if(condition) {
            delete condition;
        }

        if(true_block) {
            delete true_block;
        }

        if(false_block) {
            delete false_block;
        }
    }
};

struct AstFn : public AstNode {
    AstSymbol *type_self = nullptr;
    AstSymbol *name      = nullptr;
    std::vector<AstDec *> params;
    AstType *return_type = nullptr;
    AstBlock *body       = nullptr;

    AstFn(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstFn, line, column) {}

    virtual ~AstFn() {
        if(type_self) {
            delete type_self;
        }

        if(name) {
            delete name;
        }

        for(auto *p : params) {
            delete p;
        }

        if(return_type) {
            delete return_type;
        }

        if(body) {
            delete body;
        }
    }
};

struct AstFnCall : public AstNode {
    AstSymbol *name = nullptr;
    std::vector<AstNode *> args;

    AstFnCall(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstFnCall, line, column) {}

    virtual ~AstFnCall() {
        if(name) {
            delete name;
        }

        for(auto *p : args) {
            delete p;
        }
    }
};

struct AstLoop : public AstNode {
    bool is_foreach = false;
    AstBlock *body  = nullptr;
    AstSymbol *name = nullptr;
    AstNode *expr   = nullptr;

    AstLoop(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstLoop, line, column) {}

    virtual ~AstLoop() {
        if(body) {
            delete body;
        }

        if(name) {
            delete name;
        }

        if(expr) {
            delete expr;
        }
    }
};

struct AstContinue: public AstNode {
    AstContinue(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstContinue, line, column) {}
};

struct AstBreak: public AstNode {
    AstBreak(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstBreak, line, column) {}
};

struct AstStruct : public AstNode {
    AstSymbol *name = nullptr;
    AstBlock *block = nullptr;

    AstStruct(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstStruct, line, column) {}

    virtual ~AstStruct() {
        if(name) {
            delete name;
        }

        if(block) {
            delete block;
        }
    }
};

struct AstImpl : public AstNode {
    AstSymbol *name = nullptr;
    AstBlock *block = nullptr;

    AstImpl(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstImpl, line, column) {}

    virtual ~AstImpl() {
        if(name) {
            delete name;
        }

        if(block) {
            delete block;
        }
    }
};

struct AstAttribute : public AstNode {
    AstSymbol *name = nullptr;
    std::vector<AstNode *> args;

    AstAttribute(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstAttribute, line, column) {}

    virtual ~AstAttribute() {
        if(name) {
            delete name;
        }

        for(auto *p : args) {
            delete p;
        }
    }
};

struct AstAffix : public AstNode {
    AstSymbol *name = nullptr;
    std::vector<AstDec *> params;
    AstType *return_type = nullptr;
    AstBlock *body       = nullptr;
    AffixType affix_type;

    AstAffix(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstAffix, line, column) {}

    virtual ~AstAffix() {
        if(name) {
            delete name;
        }

        for(auto *p : params) {
            delete p;
        }

        if(return_type) {
            delete return_type;
        }

        if(body) {
            delete body;
        }
    }
};

struct AstReturn : public AstNode {
    AstNode *expr = nullptr;

    AstReturn(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstReturn, line, column) {}

    virtual ~AstReturn() {
        if(expr) {
            delete expr;
        }
    }
};

struct AstUnaryExpr : public AstNode {
    std::string op;
    AstNode *expr = nullptr;

    AstUnaryExpr(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstUnaryExpr, line, column) {}

    virtual ~AstUnaryExpr() {
        if(expr) {
            delete expr;
        }
    }
};

struct AstBinaryExpr : public AstNode {
    std::string op;
    AstNode *lhs = nullptr, *rhs = nullptr;

    AstBinaryExpr(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstBinaryExpr, line, column) {}

    virtual ~AstBinaryExpr() {
        if(lhs) {
            delete lhs;
        }

        if(rhs) {
            delete rhs;
        }
    }
};

struct AstIndex : public AstNode {
    AstNode *array = nullptr, *expr = nullptr;

    AstIndex(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstIndex, line, column) {}

    virtual ~AstIndex() {
        if(array) {
            delete array;
        }

        if(expr) {
            delete expr;
        }
    }
};

struct AstExtern : public AstNode {
    std::vector<AstFn *> decls;

    AstExtern(unsigned int line = 0, unsigned int column = 0):
        AstNode(AstNodeType::AstExtern, line, column) {}

    virtual ~AstExtern() {
        for(auto *p : decls) {
            delete p;
        }
    }
};

struct Ast {
    AstBlock *root = nullptr;

    ~Ast() {
        if(root) {
            delete root;
        }
    }
};

#endif /* AST_H */
