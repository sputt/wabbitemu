Imports System.Windows.Media.Effects

Public Class TintEffect
    Inherits ShaderEffect

    Private Shared _Shader As New PixelShader()

    Shared Sub New()
        _Shader.UriSource = MakePackUri("Effects/Tint.ps")
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
        ShaderEffect.RegisterPixelShaderSamplerProperty("Input", GetType(TintEffect), 0)

    Public Property Color As Color
        Get
            Return GetValue(ColorProperty)
        End Get

        Set(ByVal value As Color)
            SetValue(ColorProperty, value)
        End Set
    End Property

    Public Shared ReadOnly ColorProperty As DependencyProperty =
        DependencyProperty.Register("Color", GetType(Color), GetType(TintEffect),
        New UIPropertyMetadata(Colors.Transparent, PixelShaderConstantCallback(0)))

End Class
