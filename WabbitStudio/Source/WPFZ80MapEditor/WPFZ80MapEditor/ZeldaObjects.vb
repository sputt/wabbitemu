Imports System.Runtime.InteropServices
Imports System.Collections.ObjectModel

<StructLayout(LayoutKind.Sequential, Pack:=1)>
Structure AZPosition
    Public X As Byte
    Public W As Byte
    Public Y As Byte
    Public H As Byte
    Public Z As Byte
    Public D As Byte
End Structure

<StructLayout(LayoutKind.Sequential, Pack:=1)>
Structure AZExoc
    Public Count As Byte
    Public Pointer As UShort
End Structure

<StructLayout(LayoutKind.Sequential, Pack:=1)>
Structure AZObject
    Public ID As Byte
    Public Flags As Byte
    Public Position As AZPosition
    Public Anim As AZExoc
End Structure

Public Class ZBaseObject
    Inherits DependencyObject

#Region "Position/Size"
    Public Shared ReadOnly XProperty As DependencyProperty = DependencyProperty.Register("X", GetType(Byte), GetType(ZBaseObject))
    Public Shared ReadOnly YProperty As DependencyProperty = DependencyProperty.Register("Y", GetType(Byte), GetType(ZBaseObject))
    Public Shared ReadOnly ZProperty As DependencyProperty = DependencyProperty.Register("Z", GetType(Byte), GetType(ZBaseObject))

    Public Shared ReadOnly WProperty As DependencyProperty = DependencyProperty.Register("W", GetType(Byte), GetType(ZBaseObject))
    Public Shared ReadOnly HProperty As DependencyProperty = DependencyProperty.Register("H", GetType(Byte), GetType(ZBaseObject))
    Public Shared ReadOnly DProperty As DependencyProperty = DependencyProperty.Register("D", GetType(Byte), GetType(ZBaseObject))

    Public Property X As Byte
        Get
            Return GetValue(XProperty)
        End Get
        Set(value As Byte)
            SetValue(XProperty, value)
        End Set
    End Property

    Public Property Y As Byte
        Get
            Return GetValue(YProperty)
        End Get
        Set(value As Byte)
            SetValue(YProperty, value)
        End Set
    End Property

    Public Property Z As Byte
        Get
            Return GetValue(ZProperty)
        End Get
        Set(value As Byte)
            SetValue(ZProperty, value)
        End Set
    End Property

    Public Property W As Byte
        Get
            Return GetValue(WProperty)
        End Get
        Set(value As Byte)
            SetValue(WProperty, value)
        End Set
    End Property

    Public Property H As Byte
        Get
            Return GetValue(HProperty)
        End Get
        Set(value As Byte)
            SetValue(HProperty, value)
        End Set
    End Property

    Public Property D As Byte
        Get
            Return GetValue(DProperty)
        End Get
        Set(value As Byte)
            SetValue(DProperty, value)
        End Set
    End Property

    Public ReadOnly Property Bounds As Rect
        Get
            Return New Rect(X, Y, W, H)
        End Get
    End Property
#End Region
#Region "Image"
    Public Shared ReadOnly ImageProperty As DependencyProperty =
        DependencyProperty.Register("Image", GetType(Integer), GetType(ZBaseObject))

    Public Property Image As Integer
        Get
            Return GetValue(ImageProperty)
        End Get
        Set(value As Integer)
            SetValue(ImageProperty, value)
        End Set
    End Property

#End Region

    Sub Move(Dx As Integer, Dy As Integer, Optional Dz As Integer = 0)
        X += Dx : Y += Dy : Z += Dz
    End Sub
End Class

Public Class ArgsCollection
    Inherits ObservableCollection(Of Object)
    Implements ICloneable

    Public Function Clone() As Object Implements System.ICloneable.Clone
        Dim Copy As New ArgsCollection
        For Each Elem In Me
            Copy.Add(Elem.Clone)
        Next
        Return Copy
    End Function
End Class

Public Class ZDefArgGenState
    Inherits ZDefArg

    Public Sub New(Name As String, Description As String)
        SetValue(ZDefArg.NameProperty, Name)
        SetValue(ZDefArg.DescriptionProperty, Description)
    End Sub

    Public Overloads Function Clone() As Object
        Dim Copy As New ZDefArgGenState(GetValue(ZDefArg.NameProperty), GetValue(ZDefArg.DescriptionProperty))
        Copy.SetValue(ZDefArg.ValueProperty, GetValue(ZDefArg.ValueProperty))
        Return Copy
    End Function
End Class

Public Class ZDefArgGraphic
    Inherits ZDefArg

    Public Shared ReadOnly GraphicsProperty = DependencyProperty.Register("Graphics", GetType(IEnumerable(Of ZeldaImage)), GetType(ZDefArg))

    Public Sub New(Name As String, Description As String)
        SetValue(ZDefArg.NameProperty, Name)
        SetValue(ZDefArg.DescriptionProperty, Description)


        SetValue(GraphicsProperty, ZeldaImages.Images)
    End Sub

    Public Overloads Function Clone() As Object
        Dim Copy As New ZDefArgGraphic(GetValue(ZDefArg.NameProperty), GetValue(ZDefArg.DescriptionProperty))
        Copy.SetValue(ZDefArg.ValueProperty, GetValue(ZDefArg.ValueProperty))
        Return Copy
    End Function
End Class

Public Class ZDefArgObjectID
    Inherits ZDefArg

    Public Shared ReadOnly ObjectIDsProperty = DependencyProperty.Register("ObjectIDs", GetType(IEnumerable(Of String)), GetType(ZDefArg))

    Public Sub New(Name As String, Description As String)
        SetValue(ZDefArg.NameProperty, Name)
        SetValue(ZDefArg.DescriptionProperty, Description)

        Dim ObjectIDs As New List(Of String)

        For Each Label In SPASMHelper.Labels
            If Char.ToUpper(Label.Key.Chars(0)) = "K" And Label.Value < &H4000 Then
                ObjectIDs.Add(Label.Key)
            End If
        Next

        ObjectIDs.Sort()
        SetValue(ObjectIDsProperty, ObjectIDs)
    End Sub

    Public Overloads Function Clone() As Object
        Dim Copy As New ZDefArgObjectID(GetValue(ZDefArg.NameProperty), GetValue(ZDefArg.DescriptionProperty))
        Copy.SetValue(ZDefArg.ValueProperty, GetValue(ZDefArg.ValueProperty))
        Return Copy
    End Function
End Class

Public Class ZDefArgObjectFlags
    Inherits ZDefArg

    Public Sub New(Name As String, Description As String)
        SetValue(ZDefArg.NameProperty, Name)
        SetValue(ZDefArg.DescriptionProperty, Description)
    End Sub

    Public Overloads Function Clone() As Object
        Dim Copy As New ZDefArgObjectFlags(GetValue(ZDefArg.NameProperty), GetValue(ZDefArg.DescriptionProperty))
        Copy.SetValue(ZDefArg.ValueProperty, GetValue(ZDefArg.ValueProperty))
        Return Copy
    End Function
End Class

Public Class ZDefArg8Bit
    Inherits ZDefArg

    Public Sub New(Name As String, Description As String)
        SetValue(ZDefArg.NameProperty, Name)
        SetValue(ZDefArg.DescriptionProperty, Description)
    End Sub

    Public Overloads Function Clone() As Object
        Dim Copy As New ZDefArg8Bit(GetValue(ZDefArg.NameProperty), GetValue(ZDefArg.DescriptionProperty))
        Copy.SetValue(ZDefArg.ValueProperty, GetValue(ZDefArg.ValueProperty))
        Return Copy
    End Function
End Class

Public Class ZDefArg
    Inherits DependencyObject
    Implements ICloneable

    Public Shared ReadOnly NameProperty = DependencyProperty.Register("Name", GetType(String), GetType(ZDefArg))
    Public Shared ReadOnly DescriptionProperty = DependencyProperty.Register("Description", GetType(String), GetType(ZDefArg))
    Public Shared ReadOnly ValueProperty = DependencyProperty.Register("Value", GetType(String), GetType(ZDefArg))

    Public IsOptional As Boolean

    Public Property Name As String
        Get
            Return GetValue(NameProperty)
        End Get
        Set(value As String)
            SetValue(NameProperty, value)
        End Set
    End Property

    Public Property Value As String
        Get
            Return GetValue(ValueProperty)
        End Get
        Set(value As String)
            SetValue(ValueProperty, value)
        End Set
    End Property

    Public Sub New(Name As String, Description As String)
        SetValue(NameProperty, Name)
        SetValue(DescriptionProperty, Description)
    End Sub

    Public Sub New()

    End Sub

    Public Function Clone() As Object Implements System.ICloneable.Clone
        Dim Copy As New ZDefArg(GetValue(NameProperty), GetValue(DescriptionProperty))
        Copy.SetValue(ValueProperty, GetValue(ValueProperty))
        Return Copy
    End Function
End Class

Public Class ZDef
    Inherits DependencyObject

    Public Shared ReadOnly NameProperty = DependencyProperty.Register("Name", GetType(String), GetType(ZDef))
    Public Shared ReadOnly MacroProperty = DependencyProperty.Register("Macro", GetType(String), GetType(ZDef))
    Public Shared ReadOnly DescriptionProperty = DependencyProperty.Register("Description", GetType(String), GetType(ZDef))
    Public Shared ReadOnly ArgsProperty = DependencyProperty.Register("Args", GetType(ArgsCollection), GetType(ZDef))
    Public Shared ReadOnly DefaultImageProperty = DependencyProperty.Register("DefaultImage", GetType(Integer), GetType(ZDef))

    Public Property Macro As String
        Get
            Return GetValue(MacroProperty)
        End Get
        Set(value As String)
            SetValue(MacroProperty, value)
        End Set
    End Property

    Public Property Args As ArgsCollection
        Get
            Return GetValue(ArgsProperty)
        End Get
        Set(value As ArgsCollection)
            SetValue(MacroProperty, value)
        End Set
    End Property

    Public Sub New(Name As String, Macro As String, Description As String)
        SetValue(NameProperty, Name)
        SetValue(MacroProperty, Macro)
        SetValue(DescriptionProperty, Description)
        SetValue(ArgsProperty, New ArgsCollection)

        ' Assemble it when you created it to determine some properties
        Dim Obj As AZObject
        ZType.FromMacro(Macro & "(0, 0)", Obj)
        SetValue(DefaultImageProperty, CInt(Obj.Anim.Pointer))
    End Sub

    Public Sub AddArg(Name As String, Description As String, Optional IsOptional As Boolean = False)
        Dim NewArg As Object
        Select Case Name
            Case "x", "y", "z", "w", "h", "d", "ac", "cc"
                NewArg = New ZDefArg8Bit(Name, Description)
            Case "zflags"
                NewArg = New ZDefArgObjectFlags(Name, Description)
            Case "type", "ztype"
                NewArg = New ZDefArgObjectID(Name, Description)
            Case "ap"
                NewArg = New ZDefArgGraphic(Name, Description)
            Case "g"
                NewArg = New ZDefArgGenState(Name, Description)
            Case Else
                NewArg = New ZDefArg(Name, Description)
        End Select
        Args.Add(NewArg)
    End Sub
End Class

Class ZType
    Public Shared Sub FromMacro(ByVal Macro As String, ByRef ZObj As Object)
        Dim data = SPASMHelper.Assemble(" " & Macro)
        Dim h = GCHandle.Alloc(data, GCHandleType.Pinned)
        ZObj = Marshal.PtrToStructure(h.AddrOfPinnedObject, ZObj.GetType())
        h.Free()
    End Sub

    Public Shared Sub FromMacro(ByVal Name As String, Args As IEnumerable(Of Object), ByRef ZObj As Object)
        Dim Macro As String = " " & Name & "("
        For i = 0 To Args.Count - 1
            If Args(i) Is Nothing Then
                i = i + 1
                Macro = Left(Macro, Len(Macro) - 1)
            Else
                Macro &= Args(i).ToString()
            End If
            If i < Args.Count - 1 Then
                Macro &= ","
            End If
        Next
        Macro &= ")"
        FromMacro(Macro, ZObj)
    End Sub
End Class

Public Class ZObject
    Inherits ZBaseObject
    Implements ICloneable

    Public Shared ReadOnly DefinitionProperty = DependencyProperty.Register("Definition", GetType(ZDef), GetType(ZObject))
    Public Shared ReadOnly ArgsProperty = DependencyProperty.Register("Args", GetType(ArgsCollection), GetType(ZObject))

    Private _Name As String

    Public Property Definition As ZDef
        Get
            Return GetValue(DefinitionProperty)
        End Get
        Set(value As ZDef)
            SetValue(DefinitionProperty, value)
        End Set
    End Property

    Public Property Args As ArgsCollection
        Get
            Return GetValue(ArgsProperty)
        End Get
        Set(value As ArgsCollection)
            SetValue(ArgsProperty, value)
        End Set
    End Property

    Private Sub FromStruct(Obj As AZObject)
        With Obj.Position
            X = .X : W = .W
            Y = .Y : H = .H
            Z = .Z : D = .D
        End With
        Image = Obj.Anim.Pointer
    End Sub

    Public Sub New()
    End Sub

    Public Sub New(Def As ZDef, ParamArray Args() As Object)
        Dim Obj As New AZObject
        ZType.FromMacro(Def.Macro, Args, Obj)
        FromStruct(Obj)
        _Name = Def.Macro
        Me.Args = Def.Args.Clone
        For i = 0 To Args.Count - 1
            Me.Args(i).Value = Args(i).ToString().ToUpper()
        Next
        Definition = Def
    End Sub

    Public Function ToMacro() As String
        Dim Result As String = Me._Name
        Result &= "("
        Dim NewArgs = (From a In Args Select a.GetValue(ZDefArg.ValueProperty)).ToList()
        For i = 0 To NewArgs.Count - 1
            Result &= NewArgs(i)
            If i <> NewArgs.Count - 1 Then
                Result &= ","
            End If
        Next
        Result &= ")"
        Return Result
    End Function

    Public Shared Function FromMacro(Scenario As Scenario, Macro As String) As ZObject
        Dim Obj As New AZObject
        ZType.FromMacro(Macro, Obj)
        Dim ZObj As New ZObject()
        ZObj.FromStruct(Obj)
        ZObj._Name = Split(Macro, "(")(0)

        ZObj.Definition = Scenario.ObjectDefs(ZObj._Name)
        ZObj.Args = ZObj.Definition.Args.Clone
        Dim Args = Split(Split(Split(Macro, "(")(1), ")")(0), ",")

        For i = 0 To Args.Count - 1
            ZObj.Args(i).Value = Args(i)
        Next
        Return ZObj
    End Function

    Public Sub UpdatePosition(X As Double, Y As Double)
        X = Math.Min(255.0, Math.Max(0.0, X))
        Y = Math.Min(255.0, Math.Max(0.0, Y))
        Dim Obj As New AZObject
        Dim NewArgs = (From a In Args Select a.GetValue(ZDefArg.ValueProperty)).Skip(2).ToList()
        NewArgs.InsertRange(0, {CByte(X), CByte(Y)})
        ZType.FromMacro(_Name, NewArgs.Cast(Of Object), Obj)
        FromStruct(Obj)
        Args(0).Value = CInt(Me.X)
        Args(1).Value = CInt(Me.Y)
    End Sub

    Sub Jump(Dx As Integer, Dy As Integer)
        Dim StartX = X
        Dim StartY = Y

        Dim NumAttempts = 1
        While StartX = X And StartY = Y And NumAttempts < 32
            UpdatePosition(X + Dx * NumAttempts, Y + Dy * NumAttempts)
            NumAttempts = NumAttempts + 1
        End While
    End Sub

    Public Function Clone() As Object Implements System.ICloneable.Clone
        Dim Copy As New ZObject
        Copy._Name = _Name
        Copy.Definition = Definition
        Copy.Args = Args.Clone
        Copy.Image = Image
        With Copy
            .X = X : .W = W
            .Y = Y : .H = H
            .Z = Z : .D = D
        End With
        Return (Copy)
    End Function
End Class