using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;

namespace FSM
{
    [Serializable]
    public class Guard
    {
        class Condition
        {
            public readonly string index;
            public bool evalValue;
            public Condition(int id)
            {
                index = "C"+id.ToString("0000")+"C";
                evalValue = false;
            }
        }
        private string _guard;
        [NonSerializedAttribute] private string _evalString = "";
        [NonSerializedAttribute] private Dictionary<string, Condition> _conditions = new Dictionary<string, Condition>();
        static readonly private string validChars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        static readonly private string operators = "&|";

        private static readonly Dictionary<string, string> operatorResults = new Dictionary<string, string>();

        [OnDeserialized()]
        internal void OnDeserializedMethod(StreamingContext context)
        {
            Expression = _guard;
        }

        public string Expression
        {
            get { return _guard; }
            set
            {
                _conditions = new Dictionary<string, Condition>();
                _guard = value.Replace(" ", "").Replace("&", " & ").Replace("|", " | ");

                _evalString = "";
                string guard = "(" + _guard + ")";
                int start = -1;
                for (int i = 0; i < guard.Length; ++i)
                {
                    if (validChars.Contains(guard[i]))
                    {
                        if (start == -1) start = i;
                    }
                    else
                    {
                        if (start != -1)
                        {
                            string cond = guard.Substring(start, i - start);
                            _evalString += GetId(cond);
                            start = -1;
                        }
                        _evalString += guard[i];
                    }
                }
                _evalString = _evalString.Replace(" ", "");
            }
        }
        public IEnumerable<string> Conditions
        {
            get 
            { 
                HashSet<string> conds= new HashSet<string>();
                foreach (var condition in _conditions)
                {
                    conds.Add(condition.Key);
                }
                return conds;
            }
        }
        public void Set(string c, bool v)
        {
            if(_conditions.ContainsKey(c)) _conditions[c].evalValue = v;
        }

        public bool Evaluate()
        {
            if (_conditions.Count == 0) return true;
            string expr = FillEval();
            int len;
            do
            {
                len = expr.Length;
                expr = expr.Replace("(0)", "0");
                expr = expr.Replace("(1)", "1");
                expr = expr.Replace("!0", "1");
                expr = expr.Replace("!1", "0");
                expr = EvaluateNextExpression(expr);
            } while (expr.Length != len);
            if (len != 1)
            {
                throw new Exception("Invalid Guard: " + _guard);
            }
            return expr.Equals("1") ? true : false;
        }
        private string GetId(string cond)
        {
            if (_conditions.ContainsKey(cond)) return _conditions[cond].index;
            Condition newC = new Condition(_conditions.Count);
            _conditions.Add(cond, newC);
            return newC.index;
        }
        private string EvaluateOperator(string ex)
        {
            if (operatorResults.Count == 0)
            {
                operatorResults.Add("0&0","0");
                operatorResults.Add("0&1","0");
                operatorResults.Add("1&0","0");
                operatorResults.Add("1&1","1");

                operatorResults.Add("0|0","0");
                operatorResults.Add("0|1","1");
                operatorResults.Add("1|0","1");
                operatorResults.Add("1|1","1");
            }
            if (operatorResults.ContainsKey(ex))
            {
                return operatorResults[ex];
            }
            return ex;
        }
        private string EvaluateNextExpression(string expr)
        {
            for (int i = 0; i < expr.Length; ++i)
            {
                if (operators.Contains(expr[i]))
                {
                    string begin = expr.Substring(0, i - 1);
                    string ex = expr.Substring(i - 1, 3);
                    string end = expr.Substring(i - 1 + 3);

                    string exprNew = begin + EvaluateOperator(ex) + end;
                    if (!exprNew.Equals(expr))
                        return exprNew;
                }
            }
            return expr;
        }

        private string FillEval()
        {
            string eval = _evalString;
            foreach (var entry in _conditions)
            {
                eval = eval.Replace(entry.Value.index, entry.Value.evalValue ? "1" : "0");
            }
            return eval;
        }
        public string GetCode(string prefix)
        {
            string code = _evalString;
            foreach (var c in _conditions)
            {
                code = code.Replace(c.Value.index, prefix + c.Key);
            }
            code = code.Replace("&", " && ");
            code = code.Replace("|", " || ");
            return code.Equals("()") ? "" : code;
        }

        internal Guard Clone()
        {
            Guard g = new Guard();
            g.Expression = Expression;
            return g;
        }
    }
}
