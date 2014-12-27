Imports System.Runtime.InteropServices
Imports System.Collections.ObjectModel
Imports System.ComponentModel
Imports System.IO

<StructLayout(LayoutKind.Sequential, Pack:=1)>
Public Structure AZPosition
    Public X As Byte
    Public W As Byte
    Public Y As Byte
    Public H As Byte
    Public Z As Byte
    Public D As Byte
End Structure

<StructLayout(LayoutKind.Sequential, Pack:=1)>
Public Structure AZExoc
    Public Count As Byte
    Public Pointer As UShort
End Structure

<StructLayout(LayoutKind.Sequential, Pack:=1)>
Public Structure AZObject
    Public ID As Byte
    Public Flags As Byte
    Public Position As AZPosition
    Public Anim As AZExoc
End Structure

<StructLayout(LayoutKind.Sequential, Pack:=1)>
Public Structure AZEnemy
    Public ID As Byte
    Public Flags As Byte
    Public Position As AZPosition
    Public Gen As Byte
    Public Com As AZExoc
    Public Anim As AZExoc
End Structure

<StructLayout(LayoutKind.Sequential, Pack:=1)>
Public Structure AZAnim
    Public Alive As Byte
    Public X As Byte
    Public W As Byte
    Public Y As Byte
    Public H As Byte
    Public Anim As AZExoc
    Public AnimData As UShort
End Structure

<StructLayout(LayoutKind.Sequential, Pack:=1)>
Public Structure AZMisc
    Public Alive As Byte
    Public X As Byte
    Public W As Byte
    Public Y As Byte
    Public H As Byte
    Public Code As UShort
    Public Aux As UShort
End Structure

Public Interface IGeneralObject(Of ZBase)
    Sub FromStruct(Obj As ZBase)
    Property Definition As ZDef
    Property Args As ArgsCollection
    Property Name As String
    Property Image As Integer
    Property X As Byte
    Property Y As Byte
    Property W As Byte
    Property H As Byte
    Property Z As Byte
    Property D As Byte
End Interface

Public Class ZBaseObject(Of ZBase As New, Base As {New, IGeneralObject(Of ZBase)})
    Implements IGeneralObject(Of ZBase)
    Implements ICloneable

    Protected _Name As String

    Public Property Name As String Implements IGeneralObject(Of ZBase).Name
        Get
            Return _Name
        End Get
        Set(value As String)
            _Name = value
        End Set
    End Property

    Public Property Definition As ZDef Implements IGeneralObject(Of ZBase).Definition
    Public Property Args As ArgsCollection Implements IGeneralObject(Of ZBase).Args

    Public Function ToMacro() As String
        Dim Result As String = Me._Name
        Result &= "("
        Dim NewArgs = (From a In Args Select a.Value).ToList()
        For i = 0 To NewArgs.Count - 1
            Result &= NewArgs(i)

            If i <> NewArgs.Count - 1 Then
                Dim RestAreEmpty As Boolean = False
                Dim j As Integer = i + 1
                Do Until j = NewArgs.Count OrElse Not (NewArgs(j) = "" Or NewArgs(j) Is Nothing)
                    j = j + 1
                Loop
                If j = NewArgs.Count Then
                    Exit For
                End If
                Result &= ","
            End If
        Next
        Result &= ")"
        Return Result
    End Function

    Protected Overridable Sub FromStruct(Obj As ZBase) Implements IGeneralObject(Of ZBase).FromStruct
    End Sub

    Private Shared Function BaseFromZBase(ZBase As ZBase) As Base
        Dim ZObj As New Base()
        ZObj.FromStruct(ZBase)
        Return ZObj
    End Function

    Public Shared Function FromMacro(Defs As Dictionary(Of String, ZDef), Macro As String) As Base
        Dim Obj As New ZBase
        ZType.FromMacro(Macro, Obj)

        Dim ZObj = BaseFromZBase(Obj)

        ZObj.Definition = Defs(Split(Macro, "(")(0))
        ZObj.Args = ZObj.Definition.Args.Clone
        Dim Args = Split(Split(Split(Macro, "(")(1), ")")(0), ",")

        If ZObj.Args.Count > 0 Then
            For i = 0 To Args.Count - 1
                ZObj.Args(i).Value = Args(i)
            Next
        End If
        Return ZObj
    End Function

    Public Shared Function FromData(Data() As Byte) As Base
        Dim h = GCHandle.Alloc(Data, GCHandleType.Pinned)
        Dim Obj As ZBase = Marshal.PtrToStructure(h.AddrOfPinnedObject, GetType(ZBase))
        h.Free()

        Dim ZObj = BaseFromZBase(Obj)
        Return ZObj
    End Function

    Public Sub UpdatePosition(X As Double, Y As Double, Optional Assemble As Boolean = True)
        X = Math.Min(255.0, Math.Max(0.0, X))
        Y = Math.Min(255.0, Math.Max(0.0, Y))
        If Assemble Then
            Dim Obj As New ZBase
            Dim NewArgs = (From a In Args Select a.Value).Skip(2).ToList()
            NewArgs.InsertRange(0, {CByte(X), CByte(Y)})
            ZType.FromMacro(_Name, NewArgs.Cast(Of Object), Obj)
            FromStruct(Obj)
            Args(0).Value = CInt(Me.X)
            Args(1).Value = CInt(Me.Y)
        Else
            Me.X = X
            Me.Y = Y
            Args(0).Value = CInt(Math.Round(X))
            Args(1).Value = CInt(Math.Round(Y))
        End If
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

    Public Sub New()
    End Sub

    Public Sub New(Def As ZDef, Args() As Object, Optional Data As Stream = Nothing)
        Definition = Def
        _Name = Def.Macro
        Me.Args = Def.Args.Clone
        For i = 0 To Args.Count - 1
            Me.Args(i).Value = Args(i).ToString().ToUpper()
        Next
        Definition = Def
        Dim Obj As New ZBase
        If Data Is Nothing Then
            ZType.FromMacro(Def.Macro, Args, Obj)
        Else
            Dim ObjData(0 To Marshal.SizeOf(Obj) - 1) As Byte
            Data.Read(ObjData, 0, ObjData.Length)
            ZType.FromData(ObjData, Obj)
        End If
        FromStruct(Obj)
    End Sub

    Public Property X As Byte Implements IGeneralObject(Of ZBase).X
    Public Property Y As Byte Implements IGeneralObject(Of ZBase).Y
    Public Property Z As Byte Implements IGeneralObject(Of ZBase).Z


    Public Property W As Byte Implements IGeneralObject(Of ZBase).W
    Public Property H As Byte Implements IGeneralObject(Of ZBase).H
    Public Property D As Byte Implements IGeneralObject(Of ZBase).D

    Public ReadOnly Property Bounds As Rect
        Get
            Return New Rect(X, Y, W, H)
        End Get
    End Property

    Public Property Image As Integer Implements IGeneralObject(Of ZBase).Image

    Sub Move(Dx As Integer, Dy As Integer, Optional Dz As Integer = 0)
        X += Dx : Y += Dy : Z += Dz
    End Sub

    Public Function Clone() As Object Implements ICloneable.Clone
        Dim Copy As New Base
        Copy.Name = _Name
        Copy.Definition = Definition
        Copy.Args = Args.Clone
        Copy.Image = Image
        With Copy
            .X = X : .W = W
            .Y = Y : .H = H
            .Z = Z : .D = D
        End With
        Return Copy
    End Function
End Class

#Region "Args collection and types"
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
        MyBase.New(Name, Description)
    End Sub

    Public Overloads Function Clone() As Object
        Dim Copy As New ZDefArgGenState(Name, Description)
        Copy.Value = Value
        Return Copy
    End Function
End Class

Public Class ZDefArgGraphic
    Inherits ZDefArg

    Public Property Graphics As IEnumerable(Of ZeldaImage)

    Public Sub New(Name As String, Description As String, Graphics As ObservableCollection(Of ZeldaImage))
        MyBase.New(Name, Description)
        Me.Graphics = Graphics
    End Sub

    Public Overloads Function Clone() As Object
        Dim Copy As New ZDefArgGraphic(Name, Description, Graphics)
        Copy.Value = Value
        Copy.Graphics = Graphics
        Return Copy
    End Function
End Class

Public Class ZDefArgObjectID
    Inherits ZDefArg

    'Public Shared ReadOnly ObjectIDsProperty = DependencyProperty.Register("ObjectIDs", GetType(IEnumerable(Of String)), GetType(ZDefArg))

    Public ObjectIDs As IEnumerable(Of String)

    Public Sub New(Name As String, Description As String)
        MyBase.New(Name, Description)
        Dim ObjectIDs As New List(Of String)

        For Each Label In SPASMHelper.Labels
            If Char.ToUpper(Label.Key.Chars(0)) = "K" And Label.Value < &H4000 Then
                ObjectIDs.Add(Label.Key)
            End If
        Next

        ObjectIDs.Sort()
    End Sub

    Public Overloads Function Clone() As Object
        Dim Copy As New ZDefArgObjectID(Name, Description)
        Copy.Value = Value
        Copy.ObjectIDs = ObjectIDs
        Return Copy
    End Function
End Class

Public Class ZDefArgObjectFlags
    Inherits ZDefArg

    Public Sub New(Name As String, Description As String)
        MyBase.New(Name, Description)
        Me.Value = "0"
    End Sub

    Public Overloads Function Clone() As Object
        Dim Copy As New ZDefArgObjectFlags(Name, Description)
        Copy.Value = Value
        Return Copy
    End Function
End Class

Public Class ZDefArgEnemyFlags
    Inherits ZDefArg

    Public Sub New(Name As String, Description As String)
        MyBase.New(Name, Description)
        Me.Value = "0"
    End Sub

    Public Overloads Function Clone() As Object
        Dim Copy As New ZDefArgEnemyFlags(Name, Description)
        Copy.Value = Value
        Return Copy
    End Function
End Class

Public Class ZDefArg8Bit
    Inherits ZDefArg

    Public Sub New(Name As String, Description As String)
        MyBase.New(Name, Description)
    End Sub

    Public Overloads Function Clone() As Object
        Dim Copy As New ZDefArg8Bit(Name, Description)
        Copy.Value = Value
        Return Copy
    End Function
End Class

Public Class ZDefArg
    Implements ICloneable

    'Public Shared ReadOnly NameProperty = DependencyProperty.Register("Name", GetType(String), GetType(ZDefArg))
    'Public Shared ReadOnly DescriptionProperty = DependencyProperty.Register("Description", GetType(String), GetType(ZDefArg))
    'Public Shared ReadOnly ValueProperty = DependencyProperty.Register("Value", GetType(String), GetType(ZDefArg))

    Public IsOptional As Boolean

    Public Property Name As String
    Public Property Description As String
    Public Property Value As String

    Public Sub New(Name As String, Description As String)
        Me.Name = Name
        Me.Description = Description
    End Sub

    Public Sub New()

    End Sub

    Public Function Clone() As Object Implements System.ICloneable.Clone
        Dim Copy As New ZDefArg(Name, Description)
        Copy.Value = Value
        Return Copy
    End Function
End Class
#End Region

Public Class ZDef
    '    Inherits DependencyObject

    'Public Shared ReadOnly NameProperty = DependencyProperty.Register("Name", GetType(String), GetType(ZDef))
    'Public Shared ReadOnly MacroProperty = DependencyProperty.Register("Macro", GetType(String), GetType(ZDef))
    'Public Shared ReadOnly DescriptionProperty = DependencyProperty.Register("Description", GetType(String), GetType(ZDef))
    'Public Shared ReadOnly ArgsProperty = DependencyProperty.Register("Args", GetType(ArgsCollection), GetType(ZDef))
    'Public Shared ReadOnly DefaultImageProperty = DependencyProperty.Register("DefaultImage", GetType(Integer), GetType(ZDef))

    Public Property Name As String
    Public Property Macro As String
    Public Property Description As String
    Public Property Args As ArgsCollection
    Public Property DefaultImage As Integer

    Public Sub New(Name As String, Macro As String, Description As String, ObjType As Type)
        Me.Name = Name
        Me.Macro = Macro
        Me.Description = Description
        Args = New ArgsCollection

        Dim M = ObjType.BaseType.GetMethod("FromMacro")

        Dim Defs As New Dictionary(Of String, ZDef)
        Defs.Add(Macro, Me)
        Dim ObjectInstance = M.Invoke(Nothing, {Defs, Macro & "(0, 0)"})
        If ObjectInstance.Image = 0 Then
            ObjectInstance = M.Invoke(Nothing, {Defs, Macro & "(0, 0, 0)"})
            If ObjectInstance.Image = 0 Then
                ObjectInstance = M.Invoke(Nothing, {Defs, Macro & "(0, 0, 0, 0)"})
            End If
        End If

        DefaultImage = ObjectInstance.Image
    End Sub

    Public Sub AddArg(Name As String, Description As String, images As ObservableCollection(Of ZeldaImage), Optional IsOptional As Boolean = False)
        Dim NewArg As Object
        Select Case Name
            Case "x", "y", "z", "w", "h", "d", "ac", "cc"
                NewArg = New ZDefArg8Bit(Name, Description)
            Case "of"
                NewArg = New ZDefArgObjectFlags(Name, Description)
            Case "ef"
                NewArg = New ZDefArgEnemyFlags(Name, Description)
            Case "type", "ztype"
                NewArg = New ZDefArgObjectID(Name, Description)
            Case "ap"
                NewArg = New ZDefArgGraphic(Name, Description, images)
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
        Dim Data = SPASMHelper.Assemble(" " & Macro)
        FromData(Data, ZObj)
    End Sub

    Public Shared Sub FromData(Data() As Byte, ByRef ZObj As Object)
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
    Inherits ZBaseObject(Of AZObject, ZObject)

    Protected Overrides Sub FromStruct(Obj As AZObject)
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
        MyBase.New(Def, Args)
    End Sub

    Public Sub New(Def As ZDef, Data As Stream, ParamArray Args() As Object)
        MyBase.New(Def, Args, Data)
    End Sub
End Class

Public Class ZMisc
    Inherits ZBaseObject(Of AZMisc, ZMisc)

    Protected Overrides Sub FromStruct(Obj As AZMisc)
        X = Obj.X : W = Obj.W
        Y = Obj.Y : H = Obj.H
    End Sub

    Public Sub New()
    End Sub

    Public Sub New(Def As ZDef, ParamArray Args() As Object)
        MyBase.New(Def, Args)
    End Sub

    Public Sub New(Def As ZDef, Data As Stream, ParamArray Args() As Object)
        MyBase.New(Def, Args, Data)
    End Sub
End Class

Public Class ZEnemy
    Inherits ZBaseObject(Of AZEnemy, ZEnemy)

    Protected Overrides Sub FromStruct(Obj As AZEnemy)
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
        MyBase.New(Def, Args)
    End Sub

    Public Sub New(Def As ZDef, Data As Stream, ParamArray Args() As Object)
        MyBase.New(Def, Args, Data)
    End Sub

End Class

Public Class ZAnim
    Inherits ZBaseObject(Of AZAnim, ZAnim)

    Protected Overrides Sub FromStruct(Obj As AZAnim)
        X = Obj.X : W = Obj.W
        Y = Obj.Y : H = Obj.H

        Image = Obj.AnimData / 32
    End Sub

    Public Sub New()
    End Sub

    Public Sub New(Def As ZDef, X As Byte, Y As Byte, Optional Data As Stream = Nothing)
        MyBase.New(Def, {X, Y}, Data)
    End Sub

End Class