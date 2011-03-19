using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Statements;
using System.Diagnostics;

namespace WabbitC
{
    static class Optimizer
    {
        static List<Symbol> symbolTable = new List<Symbol>();
        public static void Optimize(ref Module module)
        {
            for (int i = 0; i < module.Declarations.Count; i++)
            {
                Declaration decl = module.Declarations[i];
                OptimizeBlock(ref decl.Code);                
            }
        }

        public static void OptimizeBlock(ref Block block)
        {
            for (int i = 0; i < block.Declarations.Count; i++)
            {
                symbolTable.Add(Symbol.Parse(block.Declarations[i]));
            }
            for (int i = 0; i < block.Statements.Count; i++)
            {
                int intTest;
                var statement = block.Statements[i];
                System.Type type = statement.GetType();
                if (type == typeof(Assignment))
                {
                    var assignment = statement as Assignment;
                    var symbol = FindSymbol(assignment.LValue);
                    symbol.IsConstant = int.TryParse(assignment.RValue.Value, out intTest);
                    symbol.Value = symbol.IsConstant ? (object) intTest : (object) assignment.RValue.Value;
                    symbol.IsVolatile = false;
                }
                else if (type == typeof(Move))
                {
                    var move = statement as Move;
                    var symbol = FindSymbol(move.RValue);
                    if (symbol == null)
                    {

                    }
                    else
                    {

                    }
                }
            }
        }

        static Symbol FindSymbol(Declaration decl)
        {
            var symbolToFind = Symbol.Parse(decl);
            var index = symbolTable.IndexOf(symbolToFind);
            if (index == -1)
                return null;
            return symbolTable[index];
        }
    }


    class Symbol
    {
        public string Name { get; set; }
        public bool IsConstant { get; set; }
        public bool IsVolatile { get; set; }
        public object Value { get; set; }

        public static Symbol Parse(Declaration statement)
        {
            var symbol = new Symbol();
            symbol.Name = statement.Name;
            return symbol;
        }

        public override bool Equals(object obj)
        {
            if (obj == null)
                return base.Equals(obj);
            var symbol = obj as Symbol;
            return symbol.Name == this.Name;
        }

        public override string ToString()
        {
            return Name;
        }
    }
}
