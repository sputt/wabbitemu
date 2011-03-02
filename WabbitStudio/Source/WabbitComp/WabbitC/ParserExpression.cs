using System;
using System.Collections.Generic;
using System.Text;

namespace WabbitC
{
    public abstract class ParserExpression
    {
        /// <summary>
        /// Actual token itself that starts this parser expression
        /// </summary>
        Token defaultToken;

        /// <summary>
        /// Number of the token in the total array
        /// </summary>
        int tokenNumber;

        /// <summary>
        /// all children within this parser expression
        /// </summary>
        protected List<ParserExpression> children = new List<ParserExpression>();

        public ParserExpression() { }

        public ParserExpression(Token token, int number)
        {
            defaultToken = token;
            tokenNumber = number;
        }

        internal abstract void ProcessExpression(List<Token> args);

        internal virtual void AddChild(ParserExpression expression)
        {
            children.Add(expression);
        }

        public override string ToString()
        {
            if (defaultToken != null)
                return defaultToken.TokenText;
            return base.ToString();
        }

        public override bool Equals(object obj)
        {
            if (obj == null)
                return false;
            ParserExpression exp = (ParserExpression) obj;
            return defaultToken == exp.defaultToken;
        }
    }

    public sealed class ParentExpression : ParserExpression
    {
        public ParentExpression() : base() { }
        ParentExpression(Token token, int number) : base(token, number) { }

        internal override void ProcessExpression(List<Token> args)
        {
            throw new NotImplementedException();
        }
    }

    public class PreprocessorExpression : ParserExpression
    {
        LineExpression arguments;
        public PreprocessorExpression(Token token, int number) : base(token, number) { }


        internal void SetArgs(LineExpression line)
        {
            arguments = line;
        }

        internal override void ProcessExpression(List<Token> args)
        {
            throw new NotImplementedException();
        }
    }

    public class FunctionExpression : ParserExpression
    {
        List<ArgumentExpression> parameters;
        List<LineExpression> lines = new List<LineExpression>();
        public FunctionExpression(Token token, int number) : base(token, number) { }

        internal override void ProcessExpression(List<Token> args)
        {
            throw new NotImplementedException();
        }

        internal void AddParams(List<ArgumentExpression> parameters)
        {
            this.parameters = parameters;
        }

        internal void AddLine(LineExpression line)
        {
            lines.Add(line);
        }
    }

    public class ArgumentExpression : ParserExpression
    {
        public ArgumentExpression(Token token, int number) : base(token, number) { }

        List<Token> parameters = new List<Token>();
        internal override void ProcessExpression(List<Token> param)
        {
            throw new NotImplementedException();
        }

        internal void AddArg(Token var)
        {
            parameters.Add(var);
        }
    }

    public class LineExpression : ParserExpression
    {
        List<Token> line = new List<Token>();
        public LineExpression(Token token, int number) : base(token, number) { }

        internal void AddChild(Token token)
        {
            line.Add(token);
        }

        internal override void ProcessExpression(List<Token> tokens)
        {
            line = tokens;
        }

        public override string ToString()
        {
            if (line.Count > 0)
                return line[0].TokenText;
            return base.ToString();
        }
    }
}
