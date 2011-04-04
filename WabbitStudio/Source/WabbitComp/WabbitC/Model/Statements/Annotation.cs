using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Annotation : ControlStatement
    {
        private string AnnotationText;

        public Annotation(string text)
        {
            AnnotationText = text;
        }

        public override string ToString()
        {
            return "//" + AnnotationText;
        }

		public override string  ToAssemblyString()
		{
			return ";" + AnnotationText;
		}
    }
}
