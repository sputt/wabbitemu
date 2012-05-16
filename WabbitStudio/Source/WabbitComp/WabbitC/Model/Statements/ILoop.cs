using System;
namespace WabbitC.Model.Statements
{
    interface ILoop
    {
        Block Body { get; set; }
        Declaration CondDecl { get; set; }
        Block Condition { get; set; }
        Label Label { get; set; }
    }
}
