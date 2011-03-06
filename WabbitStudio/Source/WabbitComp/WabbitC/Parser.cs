using System;
using System.Collections.Generic;
using System.Text;

#region Class layout
/*
 * class Module
 *   Type[]         ; prototypes, structs, typedefs
 *   Declaration[]  ; globals, functions
 * 
 * class Type
 *   public Size
 *   public Layout
 *   ...
 * 
 * Function : 
 * 
 * class Declaration
 *   Property Type
 *   Public Value InitialValue
 *  
 * class Variable
 *   Private Declaration
 *   
 * class Value
 *   Property Type
 *   Union {Declaration, Immediate}       
 * 
 * class Statement
 * 
 * class ValueStatement : Statement
 * 
 * class ControlStatement : Statement
 * 
 * class LoneValue : ValueStatement
 *   Public Value
 * 
 * class Assignment : ValueStatement
 *   Public Variable Assignee
 *   Public Value ValueStatement
 *   
 * class Addition : ValueStatement
 *   Public Variable Accumulator
 *   Public Value
 *   
 * class Multiply : ValueStatement
 *   Public Variable Accumulator
 *   Public Value
 * 
 * class If : ControlStatement
 *   Public Statement Condition
 *   Public Block IfTrue
 *   Optional Public Block IfFalse
 *   
 * class Loop : ControlStatement
 *   Public ValueStatement Condition
 *   Public Block Body
 * 
 * class Block
 *  Type[]
 *  Declaration[]
 *  Statement[]
 *   
 * class Function
 *   Block
 * 
 * 
 * Example expression:
 * 
 * 
 * int test = 4;
 * int bob = (test + 2) * 2;
 * 
 * Assignment(test, Value(4))
 * Assignment(temp1, Value(test))
 * Addition(temp1, Value(2))
 * Multiplication(temp1, Value(2))
 * Assignment(bob, Value(temp1))
*/
#endregion



namespace WabbitC
{
    public class OldParser
    {
        /*private List<Token> tokens;
        public OldParser()
        {
        }

        ParserExpression parserHierarchy = new ParentExpression();

        public void ParseTokens(List<Token> tokens)
        {
            this.tokens = tokens;
            for (int i = 0; i < tokens.Count; i++)
                HandleStartToken(ref i, ref parserHierarchy);
        }

        private ParserExpression HandleStartToken(ref int i, ref ParserExpression parent)
        {
            switch (tokens[i].Type)
            {
                case TokenType.CommentType:
                    return null;
                case TokenType.Preprocessor:
                    return HandlePreprocessor(ref i, ref parent);
                case TokenType.ReservedKeyword:
                case TokenType.StringType:
                    return HandleFunctionOrVariable(ref i, ref parent);
            }
            return null;
        }

        private ParserExpression HandleFunctionOrVariable(ref int i, ref ParserExpression parent)
        {
            List<Token> wholeLine = new List<Token>();
            int startLine = i;
            bool hasParen = false;
            while (tokens[i].Text != ";" && tokens[i].Text != "{")
            {
                if (tokens[i].Text == "(")
                    hasParen = true;
                wholeLine.Add(tokens[i++]);
            }
            if (hasParen && tokens[i].Text == "{")
            {
                i = startLine;
                return HandleFunction(ref i, ref parent);
            }
            LineExpression line = new LineExpression(tokens[i], i);
            foreach (Token token in wholeLine)
                line.AddChild(token);
            return line;
        }

        private FunctionExpression HandleFunction(ref int i, ref ParserExpression parent)
        {
            var function = new FunctionExpression(tokens[i], i);
            var funcParent = (ParserExpression)function;
            List<ArgumentExpression> parameters = new List<ArgumentExpression>();
            Token type = tokens[i++];
            Token name = tokens[i++];
            Token paren = tokens[i++];
            while (tokens[i].Text != ")")
            {
                var param = new ArgumentExpression(tokens[i], i);
                while (tokens[i].Text != ")" && tokens[i].Text != ",")
                    param.AddArg(tokens[i++]);
                parameters.Add(param);
            }
            i++;        //skip )
            //handle old C style shit
            if (tokens[i].Text != "{")
                parameters.Clear();
            while (tokens[i].Text != "{")
            {
                var param = new ArgumentExpression(tokens[i], i);
                var line = GetLine(ref i, ref funcParent);
                i++;
                param.AddChild(line);
                parameters.Add(param);
            }
            function.AddParams(parameters);
            i++;            //skip {
            
            //now we handle the stuff between the braces
            while (tokens[i].Text != "}")
            {
                var line = (LineExpression) GetLine(ref i, ref funcParent);
                if (line != null)
                    function.AddLine(line);
                i++;
            }
            i++;        //skip }
            parent.AddChild(function);
            return function;
        }

        private ParserExpression GetLine(ref int i, ref ParserExpression parent)
        {
            return HandleStartToken(ref i, ref parent);

			//List<Token> line = new List<Token>();
			//while (tokens[i].TokenText != ";")
			//    line.Add(tokens[i++]);
			//return line;
        }

        private PreprocessorExpression HandlePreprocessor(ref int i, ref ParserExpression parent)
        {
            PreprocessorExpression expression = new PreprocessorExpression(tokens[i], i);
            switch (tokens[i].Text)
            {
                case "#if":
                case "#ifdef":
                case "#ifndef":
                case "#elsif":
                    HandleIf(ref i, ref expression);
                    break;
                case "#undefine":
                case "#define":
                    HandleDefine(ref i, ref expression);
                    break;
                case "#include":
                    HandleInclude(ref i, ref expression);
                    break;
                case "#endif":
                case "#else":
                    break;
                case "#region":
                case "#error":
                case "#warning":
                    HandleMessage(ref i, ref expression);
                    break;
            }
            parent.AddChild(expression);
            return expression;
        }

        private void HandleMessage(ref int i, ref PreprocessorExpression exp)
        {
            int currentLine = tokens[i].LineNumber;
            LineExpression line = new LineExpression(tokens[i], i);
            while (tokens[++i].LineNumber == currentLine)
                line.AddChild(tokens[i]);
            exp.AddChild(line);
            i--;
        }

        private void HandleDefine(ref int i, ref PreprocessorExpression exp)
        {
            int currentLine = tokens[i++].LineNumber, endLineNum;
            bool isMultiLine;
            do
            {
                LineExpression line = new LineExpression(tokens[i], i);
                isMultiLine = false;
                while (tokens[i].LineNumber == currentLine)
                    line.AddChild(tokens[i++]);
                endLineNum = i--;
                while (tokens[i].Type == TokenType.CommentType)
                    i--;
                if (tokens[i].Text == "\\")
                    isMultiLine = true;
                i = endLineNum;
                currentLine = tokens[i].LineNumber;
                exp.AddChild(line);
            } while (isMultiLine);
            i--;
        }

        private void HandleIf(ref int i, ref PreprocessorExpression exp)
        {
            int currentLine = tokens[i].LineNumber;
            LineExpression line = new LineExpression(tokens[i], i);
            while (tokens[++i].LineNumber == currentLine)
                line.AddChild(tokens[i]);
            exp.SetArgs(line);
            ParserExpression internalLine = null, parent = (ParserExpression) exp;
            LineExpression expToCheck = new LineExpression(new Token() { Text = "#endif", Type = WabbitC.TokenType.Preprocessor }, -1);
            while (!(internalLine != null && internalLine.Equals(expToCheck)))
            {
                internalLine = GetLine(ref i, ref parent);
                i++;            //like the for loop does
            }
            i--;
        }

        private void HandleInclude(ref int i, ref PreprocessorExpression exp)
        {
            LineExpression line = new LineExpression(tokens[i], i);
            if (tokens[++i].Text == "<")
            {
                //#include <>
                while (tokens[i].Text != ">")
                    line.AddChild(tokens[i++]);
                line.AddChild(tokens[i]);
            }
            else
                //#include ""
                line.AddChild(tokens[i]);
            exp.AddChild(line);
        }*/
    }
}
