using System.Collections.Generic;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Annotations;

namespace Revsoft.Wabbitcode.Services
{
    [UsedImplicitly]
    public class MacroService : IMacroService
    {
        private readonly List<WabbitcodeMacro> _macros = new List<WabbitcodeMacro>();

        private WabbitcodeMacro _currentMacro;

        public bool IsRecording
        {
            get { return _currentMacro != null; }
        }

        public void StartRecordingMacro(string macroName)
        {
            _currentMacro = new WabbitcodeMacro();
        }

        public void StopRecordingMacro()
        {
            _macros.Add(_currentMacro);
            _currentMacro = null;
        }

        public void RecordKeyData(Keys keyData)
        {
            _currentMacro.AddKey(keyData);
        }
    }

    public class WabbitcodeMacro
    {
        private readonly List<Keys> _keyData = new List<Keys>();

        public void AddKey(Keys keyData)
        {
            _keyData.Add(keyData);
        }
    }

    public interface IMacroService
    {
        bool IsRecording { get; }

        void StartRecordingMacro(string macroName);

        void StopRecordingMacro();

        void RecordKeyData(Keys keyData);
    }
}