/* Implementation of Recursive-Descent Parser
 * parse.cpp
 * Programming Assignment 2
 * Spring 2022
*/

#include "parseInt.h"
#include<algorithm>
#include<map>
#include<vector>
#include <queue>

vector<string> defiVariableList;
vector<string> identifiersV;
map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults;
queue<Value> *ValQue;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;
	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);


//Program is: Prog = PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line)
{
	bool prog1, prog2;
	LexItem tok = Parser::GetNextToken(in, line);
		
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			
			tok = Parser::GetNextToken(in, line);
			if (tok.GetToken() == SEMICOL) {
				prog1 = DeclBlock(in, line); 
			
				if(prog1) {
					prog2 = ProgBody(in, line);
					if(!prog2)
					{
						ParseError(line, "Mistake in PROG body.");
						return false;
					}
					
					return true;//Successful Parsing is completed
				}
				else
				{
					ParseError(line, "Incorrect DECL here.");
					return false;
				}
			}
			else
			{
				//Parser::PushBackToken(tok);
				ParseError(line-1, "Missing SEMICOL here.");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing PROG name here.");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else if(tok.GetToken() == DONE && tok.GetLinenum() <= 1){
		ParseError(line, "File is empty");
		return true;
	}
	ParseError(line, "Missing PROGRAM.");
	return false;
}

bool ProgBody(istream& in, int& line){
	bool curBool;
		
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok.GetToken() != BEGIN) {
		ParseError(line, "Non-recognizable PROG body here.");
		return false;
	}
	else
	{
        curBool = Stmt(in, line);
		
		while(curBool)
		{
			tok = Parser::GetNextToken(in, line);
			if(tok != SEMICOL)
			{
				line--;
				ParseError(line, "Missing semicolon in Statement.");
				return false;
			}
			
			curBool = Stmt(in, line);
		}
			
		tok = Parser::GetNextToken(in, line);
		if(tok != END )
		{
		    ParseError(line, "Syntactic error in Program Body.");
			return false;	
		}
		else 
		{
            return true;
			
		}
		
	}	
}//End of ProgBody function


bool DeclBlock(istream& in, int& line) {
	bool curBool = false;
	LexItem tok;
	//cout << "in Decl" << endl;
	LexItem t = Parser::GetNextToken(in, line);
	if(t == VAR)
	{
		curBool = DeclStmt(in, line);
		
		while(curBool)
		{
			tok = Parser::GetNextToken(in, line);
			if(tok != SEMICOL)
			{
				line--;
				ParseError(line, "Missing SEMCIOL in Declaration Stmt.");
				return false;
			}
            else
            {
			curBool = DeclStmt(in, line);
            }
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok == BEGIN )
		{
			Parser::PushBackToken(tok);
			return true;
		}
		else
		{
			ParseError(line, "Syntax error in DECL block.");
			return false;
		}
	}
	else
	{
		ParseError(line, "Non-recognizable DECL block.");
		return false;
	}
	
}//end of DeclBlock function

bool DeclStmt(istream& in, int& line)
{
    bool finalB = true;
	LexItem curLex;
	bool curBool = IdentList(in, line);
	
	if (curBool == true)
	{

	}
	else
	{
		ParseError(line, "Incorrect variable in DECL block.");
		return curBool;
	}
	
	curLex = Parser::GetNextToken(in, line);
	if(curLex != COLON)
	{
		Parser::PushBackToken(curLex);
		return false;
	}
	else
	{
		curLex = Parser::GetNextToken(in, line);
		if( curLex == STRING ||curLex == REAL || curLex == INTEGER)
		{
			//YOU CHANGED THIS BAEEEE
			for(string identVal : identifiersV) 
			{
				SymTable[identVal] = curLex.GetToken();
			}
			identifiersV.clear();
			return true;
		}
		else
		{
			ParseError(line, "Incorrect Declaration Type.");
			return false;
		}
        return finalB;
	}
	
}//End of DeclStmt

//IdList:= IDENT {,IDENT}
bool IdentList(istream& in, int& line) {
	bool curBool = false;
	string IDstr;
	
	LexItem curLex = Parser::GetNextToken(in, line);
	if(curLex != IDENT)
	{
		Parser::PushBackToken(curLex);
		return true;	
	}
	else
	{
		identifiersV.push_back(curLex.GetLexeme());
		IDstr = curLex.GetLexeme();
		if ((defVar.find(IDstr)->second))
		{
			ParseError(line, "Variable being defined again");
			return false;
		}	
		else if (!(defVar.find(IDstr)->second))
		{
			defVar[IDstr] = true;
           // return true;
		
		}
	
	}
	
	curLex = Parser::GetNextToken(in, line);
	
	if (curLex == COLON) {
		Parser::PushBackToken(curLex);
		return true;
	}
	else if(curLex == COMMA)
	{
		curBool = IdentList(in, line);
	}
	else 
    {
		ParseError(line, "Unrecognized pattern here");
		cout << "(" << curLex.GetLexeme() << ")" << endl;
		return false;
	}
	return curBool;
	
}//End of IdentList
	

//Stmt is either a WriteLnStmt, ForepeatStmt, IfStmt, or AssigStmt
//Stmt = AssigStmt | IfStmt | WriteStmt | ForStmt 
bool Stmt(istream& in, int& line) {
	bool curBool;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	default:
		Parser::PushBackToken(t);
		return false;
    case IDENT:
		Parser::PushBackToken(t);
        curBool = AssignStmt(in, line);
		
		break;
	case IF:
		curBool = IfStmt(in, line);
		break;
    case WRITELN:
		curBool = WriteLnStmt(in, line);
		
		break;
	}

	return curBool;
}//End of Stmt


bool WriteLnStmt(istream& in, int& line) {
	LexItem t;
	//cout << "in WriteStmt" << endl;
	ValQue = new queue<Value>;
	t = Parser::GetNextToken(in, line);
	if( t == LPAREN ) 
    {
        
    }
    else 
    {
		ParseError(line, "Missing LPAREN");
		return false;
	}
	
	bool curBool = ExprList(in, line);
	
	if( curBool ) {
		
	}
    else
    { 
        ParseError(line, "Missing expr after WriteLN");
		return false;
    }
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing RPAREN");
		return false;
	}
	
	
	//Evaluate: print out the list of expressions' values
	while (!(*ValQue).empty())
	{
		Value newV = (*ValQue).front();
		cout << newV;
		ValQue->pop();
	}
	cout << endl;

	return curBool;
}//End of WriteLnStmt

//IfStmt:= if (Expr) then Stm} [Else Stmt]
bool IfStmt(istream& in, int& line) {
    Value retVal;
    bool finalB = true;
	bool curBool=true; 
	LexItem curLex;
	
	curLex = Parser::GetNextToken(in, line);
	if( curLex == LPAREN ) 
	{

	}
	else
	{
		ParseError(line, "Missing LPAREN");
		return false;
	}
	
	curBool = LogicExpr(in, line, retVal);
	if(curBool) 
	{

	}
	else{
		ParseError(line, "Missing if in logic expr");
		return false;
	}
	curLex = Parser::GetNextToken(in, line);
	if(curLex != RPAREN ) {
		
		ParseError(line, "Missing RPAREN");
		return false;
	}
	// Add stuff to check conditional for if stmt
	curLex = Parser::GetNextToken(in, line);
	if(curLex == THEN)
	{
		
	}
    else
    {
        ParseError(line, "If syntax err");
		return false;
    }
	if (retVal.GetBool())
	{
		curBool = Stmt(in, line);
		if(curBool)
		{

		}
		else {
			ParseError(line, "Missing stmt for If and Then part");
			return false;
		}
		while (curLex != SEMICOL)
		{
				curLex = Parser::GetNextToken(in, line);
		}
		Parser::PushBackToken(curLex);
		return true;
	}
	if (retVal.GetBool() == false)
	{
		while (curLex != ELSE && curLex != SEMICOL)
		{
				curLex = Parser::GetNextToken(in, line);
		}
		Parser::PushBackToken(curLex);
	}
	curLex = Parser::GetNextToken(in, line);
	if( curLex != ELSE ) {
		
	}
    else
    {
      curBool = Stmt(in, line);
		if(!curBool)
		{
			ParseError(line, "Missing stmt for If-Stmt and/or Else-Part");
			return false;
		}
		return true;  
    }
    
		
	Parser::PushBackToken(curLex);
	return finalB;
}//End of IfStmt function



//Var:= ident
bool Var(istream& in, int& line, LexItem& idtok)
{
    bool finalB = false;
	//called only from the AssignStmt function
	string IDstr;
	
	idtok = Parser::GetNextToken(in, line);
	if(idtok.GetToken() != ERR)
	{

	}
	else
	{
		ParseError(line, "Unrecognized input");
		cout << "(" << idtok.GetLexeme() << ")" << endl;
		return false;
	}
	if (idtok == IDENT){
		IDstr = idtok.GetLexeme();
		
		if ((defVar.find(IDstr)->second))
		{
			
		}	
		else 
		{
			ParseError(line, "Undeclared var here");
			return false;
		}
		return true;
	}
	
	return finalB;
}//End of Var

//AssignStmt:= Var = Expr
bool AssignStmt(istream& in, int& line) {
	Value retVal;
	bool varcheck = false;
    LexItem ctTok;
    bool curBool = true;
	LexItem tok;

	
	varcheck = Var( in, line, ctTok);
	
	
	if (varcheck){
		tok = Parser::GetNextToken(in, line);
		
		if (tok == ASSOP)
		{
			curBool = Expr(in, line, retVal);
			if(!curBool) 
			{
				ParseError(line, "Missing Expression in Assignment Statment");
				return curBool; 
			}
			if (SymTable[ctTok.GetLexeme()] == REAL)
			{
				if (retVal.GetType() != VINT)
				{

				}
				else{
					float currFloat = (float)retVal.GetInt();
					retVal = currFloat;
					TempsResults[ctTok.GetLexeme()].SetType(VREAL);
				}
				if (retVal.GetType() != VSTRING)
				{

				}
				else{
					ParseError(line, "Invalid Type with reals and strings");
					return false;
				}
			}
            if (SymTable[ctTok.GetLexeme()] == STRING)
			{
				if (retVal.GetType() == VSTRING)
				{
					
				}
				else 
				{
					ParseError(line, "Invalid Type error with strings");
					return false;
				}
			}
			
			if (SymTable[ctTok.GetLexeme()] == INTEGER)
			{
				if (retVal.GetType() != VREAL)
				{
					
				}
				else
				{
					retVal = (int)retVal.GetReal();
					TempsResults[ctTok.GetLexeme()].SetType(VINT);
				}
				
				if (retVal.GetType() != VSTRING)
				{

				}
				else 
				{
					ParseError(line, "Invalid Type with strings and integers");
					return false;
				}
			}
			

			TempsResults[ctTok.GetLexeme()] = retVal;
			defVar[ctTok.GetLexeme()] = true;
			defiVariableList.push_back(ctTok.GetLexeme());
		}
        else if (tok != ASSOP)
        {
            
        }
		else if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assign here");
			return false;
		}
	}
	else {
		ParseError(line, "Missing LHS Variable in Assignment stmt");
		return false;
	}
	return curBool;	
}

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool curBool = false;
	Value retVal;
	curBool = Expr(in, line, retVal);
	if(curBool){
		
	}
	else{
		ParseError(line, "Missing expr");
		return false;
	}
	ValQue->push(retVal);
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok != COMMA)
    {
        
    }
    if (tok == COMMA){
		
		curBool = ExprList(in, line);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return curBool;
}

//Expr:= Term {(+|-) Term}
bool Expr(istream& in, int& line, Value & retVal) {
	Value expr1, expr2;
    bool finalB = true;
	//cout << "in Expr" << endl;
	bool sf = Term(in, line, expr1);
	LexItem curLex;
	
	if( !sf ) 
    {
		return false;
	}
	retVal = expr1;
	
	curLex = Parser::GetNextToken(in, line);
	if(curLex.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << curLex.GetLexeme() << ")" << endl;
		return false;
	}
	//Evaluate: evaluate the expression for addition or subtraction
	while ( curLex == MINUS || curLex == PLUS ) 
	{
		sf = Term(in, line, expr2);
		if( !sf ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
        else if(curLex == MINUS)
		{
			retVal = retVal - expr2;
			if(retVal.IsErr())
			{
				ParseError(line, "Illegal - ops here.");
				return false;
			}
		}
		
		if(curLex == PLUS)
		{
			retVal = retVal + expr2;
			if(retVal.IsErr())
			{
				ParseError(line, "Illegal + ops here.");
				//cout << "(" << tok.GetLexeme() << ")" << endl;		
				return false;
			}
		}
			
		curLex = Parser::GetNextToken(in, line);
        if(curLex.GetToken() != ERR)
        {
        }
		if(curLex.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << curLex.GetLexeme() << ")" << endl;
			return false;
            
		}			
	}
	Parser::PushBackToken(curLex);
	return finalB;
}//end of Expr

//Term:= SFactor {(*|/) SFactor}
bool Term(istream& in, int& line, Value & retVal) {
	Value term1, term2;
    bool finalB = true;
	//cout << "in Expr" << endl;
	bool sf = SFactor(in, line, term1);
	LexItem tok;
	
	if( !sf ) 
    {
		return false;
	}
    else
    {
	retVal = term1;
    }
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
    else
    {
        
    }
	//Evaluate: evaluate the expression for addition or subtraction
	while (  tok == DIV || tok == MULT ) 
	{
		sf = SFactor(in, line, term2);
		if( !sf ) 
		{
			ParseError(line, "Missing ops after operator");
			return false;
		}
		
		if(tok == MULT)
		{
			retVal = retVal * term2;
			if(retVal.IsErr())
			{
				ParseError(line, "Illegal times operation.");
				//cout << "(" << tok.GetLexeme() << ")" << endl;		
				return false;
			}
            else
            {
                
            }
		}
		else if(tok == DIV)
		{
			if (term2.IsInt())
			{
				if (term2.GetInt() == 0)
				{
					ParseError(line, "Running  Error because of Division by Zero");
					return false;
				}
                else if (term2.GetInt() != 0)
                {
                    
                }
			}
			else if (term2.IsReal())
			{
				if (term2.GetReal() == 0.0)
				{
					ParseError(line, "Run-Time Error-Illegal because of division by real 0.0");
					return false;
				}
                else if (term2.GetReal() != 0.0)
                {
                    
                }
			}
			retVal = retVal / term2;
			if(retVal.IsErr())
			{
				ParseError(line, "Illegal division operation.");
				return false;
			}
            else
            {
                return true;
            }
		}
			
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
		
	}
	Parser::PushBackToken(tok);
	return finalB;
}//end of Expr

//SFactor = Sign Factor | Factor
bool SFactor(istream& in, int& line, Value& retVal)
{
	LexItem t = Parser::GetNextToken(in, line);
	bool curBool;
	int sign = 8;
	if(t == MINUS )
	{
		sign = -1;
	}
	else if(t == PLUS)
	{
        LexItem tok = Parser::GetNextToken(in, line);
        if (tok != SCONST)
        {
            Parser::PushBackToken(tok);
        }
        else
        {
            ParseError(line, "Illegal combination of sign and string");
            return false;
        }
		sign = 1;
        
	}
	else
		Parser::PushBackToken(t);
		
	curBool = Factor(in, line, sign, retVal);
	return curBool;
}

//LogicExpr = Expr (== | <) Expr
bool LogicExpr(istream& in, int& line, Value& retVal)
{
    bool finalB = true;
    Value lo1;
    Value lo2;
    bool sf = Expr(in, line, lo1);
    LexItem tok;
    
    if( !sf ) {
        return false;
    }
	retVal = lo1;
    
    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern here");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    if ( tok == EQUAL ||tok == GTHAN || tok == LTHAN)
    {
        sf = Expr(in, line, lo2);
        if( !sf ) 
        {
            ParseError(line, "Missing expr after relational ops here");
            return false;
        }
		if (tok == GTHAN)
		{
			retVal = lo1 > lo2;
			if(retVal.IsErr())
			{
				ParseError(line, "Illegal > than oper here.");		
				return false;
			}
			return true;
		}
		else if (tok == LTHAN)
		{
			retVal = lo1 < lo2;
			if(retVal.IsErr())
			{
				ParseError(line, "Illegal < than oper.");		
				return false;
			}
			return true;
		}
		else if (tok == EQUAL)
		{
			retVal = (lo1 == lo2);
			if(retVal.IsErr())
			{
				ParseError(line, "Illegal = to oper here.");		
				return false;
			}
			return true;
		}
    }
    return finalB;
}

//Factor := ident | iconst | rconst | sconst | (Expr)
bool Factor(istream& in, int& line, int sign, Value& retVal) {
	bool finalB = true;
    if (sign == -1)
	{
		retVal = retVal * -1;
	}
	LexItem tok = Parser::GetNextToken(in, line);
	string rvFactor = tok.GetLexeme();
	if( tok == IDENT ) {
		if (!(defVar.find(rvFactor)->second) || ( find(defiVariableList.begin(), defiVariableList.end(), rvFactor) == defiVariableList.end() ))
		{
			ParseError(line, "Using Undefined IDENT var here");
			return false;	
		}
        else 
        {
            
        }
		if (sign == -1)
		{
			retVal = TempsResults[rvFactor] * -1; // TempsResults o
		}
		else 
		{
			retVal = TempsResults[rvFactor]; // TempsResults o
		}
		return true;
	}
	
	else if( tok == SCONST ) 
	{
		retVal = rvFactor;
		return true;
	}
	else if( tok == RCONST)
	{
		retVal = stof(rvFactor);
		if (sign == -1)
		{
			retVal = retVal * -1;
		}
		return true;
	}
    else if( tok == ICONST ) 
	{
		retVal = stoi(rvFactor);
		if (sign == -1)
		{
			retVal = retVal * -1;
		}
		return true;
	}
    else if (SymTable[rvFactor] == REAL) 
    {
        retVal = stof(rvFactor);
		if (sign == -1)
		{
			retVal = retVal * -1;
		}
		return true;
    }
	else if( tok == LPAREN ) {
		bool check = Expr(in, line, retVal);
		if( !check ) {
			ParseError(line, "Missing expr after ( here");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return check;
		else 
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression here");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern here");
		cout << "(" << rvFactor << ")" << endl;
		return false;
	}

	return finalB;
}