namespace Wabbitcode
{
    using System;
    using System.Drawing;

    public enum DescriptorRecognition
    {
        /// <summary>
        /// Only if the whole token is equal to the word
        /// </summary>
        WholeWord,
        /// <summary>
        /// If the word starts with the token
        /// </summary>
        StartsWith,
        /// <summary>
        /// If the word contains the Token
        /// </summary>
        Contains
    }

    public enum DescriptorType
    {
        /// <summary>
        /// Causes the highlighting of a single word
        /// </summary>
        Word,
        /// <summary>
        /// Causes the entire line from this point on the be highlighted, regardless of other tokens
        /// </summary>
        ToEOL,
        /// <summary>
        /// Highlights all text until the end token;
        /// </summary>
        ToCloseToken
    }

    public class HighlightDescriptor
    {
        public readonly string CloseToken;
        public readonly Color Color;
        public readonly DescriptorRecognition DescriptorRecognition;
        public readonly DescriptorType DescriptorType;
        public readonly Font Font;
        public readonly string Token;
        public readonly bool UseForAutoComplete;

        public HighlightDescriptor(string token, Color color, Font font, DescriptorType descriptorType, DescriptorRecognition dr, bool useForAutoComplete)
        {
            if (descriptorType == this.DescriptorType.ToCloseToken)
            {
                throw new ArgumentException("You may not choose ToCloseToken DescriptorType without specifing an end token.");
            }

            this.Color = color;
            this.Font = font;
            this.Token = token;
            this.DescriptorType = descriptorType;
            this.DescriptorRecognition = dr;
            this.CloseToken = null;
            this.UseForAutoComplete = useForAutoComplete;
        }

        public HighlightDescriptor(string token, string closeToken, Color color, Font font, DescriptorType descriptorType, DescriptorRecognition dr, bool useForAutoComplete)
        {
            this.Color = color;
            this.Font = font;
            this.Token = token;
            this.DescriptorType = descriptorType;
            this.CloseToken = closeToken;
            this.DescriptorRecognition = dr;
            this.UseForAutoComplete = useForAutoComplete;
        }
    }
}