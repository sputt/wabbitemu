Imports System.Windows.Media.Effects

Public Class InvertEffect
    Inherits ShaderEffect

    Private Shared _Shader As New PixelShader()

    Shared Sub New()
        _Shader.UriSource = MakePackUri("Effects/Invert.ps")
    End Sub

    Private Shared Function MakePackUri(File As String) As Uri
        Dim Asm = GetType(TintEffect).Assembly
        Dim ShortName = Asm.ToString().Split(",")(0)
        Dim Uri = "pack://application:,,,/" & ShortName & ";component/" & File

        Return New Uri(Uri)
    End Function

    Public Sub New()
        PixelShader = _Shader
        UpdateShaderValue(InputProperty)
    End Sub

    Public Property Input As Brush
        Get
            Return GetValue(InputProperty)
        End Get

        Set(ByVal value As Brush)
            SetValue(InputProperty, value)
        End Set
    End Property

    Public Shared ReadOnly InputProperty As DependencyProperty =
        ShaderEffect.RegisterPixelShaderSamplerProperty("Input", GetType(InvertEffect), 0)

End Class
