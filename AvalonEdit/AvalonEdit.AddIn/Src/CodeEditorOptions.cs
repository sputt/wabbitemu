using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.AvalonEditExtension.Interface;

namespace Revsoft.Wabbitcode.AvalonEditExtension
{
    /// <summary>
    /// Class containing static properties for the code completion options.
    /// </summary>
    public class CodeEditorOptions : ITextEditorOptions
    {
        public static CodeEditorOptions Instance = new CodeEditorOptions();
        /// <summary>
        /// Global option to turn all code-completion-related features off.
        /// </summary>
        public static bool EnableCodeCompletion { get; set; }

        public static bool DataUsageCacheEnabled { get; set; }

        public static int DataUsageCacheItemCount { get; set; }

        public static bool TooltipsEnabled { get; set; }

        public static bool TooltipsOnlyWhenDebugging { get; set; }

        public static bool KeywordCompletionEnabled { get; set; }

        public static bool CompleteWhenTyping { get; set; }

        public static bool InsightEnabled { get; set; }

        public static bool InsightRefreshOnComma { get; set; }
    }
}
