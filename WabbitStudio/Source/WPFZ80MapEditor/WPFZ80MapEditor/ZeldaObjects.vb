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

Public Interface IBaseGeneralObject
    Property PreviousVersion As IBaseGeneralObject

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

    ReadOnly Property Bounds As Rect

    Sub Update()
    Sub UpdatePosition(X As Double, Y As Double)
    Sub UpdatePosition(X As Double, Y As Double, W As Double, H As Double)

    Property IsInitializating As Boolean

    Property NamedSlot As String

End Interface

Public Interface IGeneralObject(Of ZBase)
    Inherits IBaseGeneralObject

    Sub FromStruct(Obj As ZBase)
End Interface

Public Class ZBaseObject(Of ZBase As New, Base As {New, IGeneralObject(Of ZBase)})
    Implements IGeneralObject(Of ZBase)
    Implements ICloneable
    Implements INotifyPropertyChanged

    Public Property PreviousVersion As IBaseGeneralObject Implements IGeneralObject(Of ZBase).PreviousVersion

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

    Private _Args As ArgsCollection
    Public Property Args As ArgsCollection Implements IGeneralObject(Of ZBase).Args
        Get
            Return _Args
        End Get
        Set(value As ArgsCollection)
            _Args = value
            _Args.Base = Me
        End Set
    End Property

    Private _NamedSlot As String
    Public Property NamedSlot As String Implements IGeneralObject(Of ZBase).NamedSlot
        Get
            Return _NamedSlot
        End Get
        Set(value As String)
            If value <> _NamedSlot Then
                _NamedSlot = value
                RaisePropertyChanged("NamedSlot")
            End If
        End Set
    End Property

    Public ReadOnly Property ArgsAndLabels As ICollection(Of Object)
        Get
            Dim Indexes = Enumerable.Range(0, Args.Count)
            Return Args.Cast(Of Object).Concat(Indexes.Select(Function(i) New ArgNameAndIndex(i, Args(i).LabelText))).ToList()
        End Get
    End Property

    Public Sub Write(Stream As StreamWriter, Slot As Integer)
        If NamedSlot IsNot Nothing Then
            Stream.WriteLine("#define " & NamedSlot & " " & Slot)
        End If
        Stream.WriteLine(vbTab & ToMacro())
    End Sub

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

        ZObj.Name = Split(Macro, "(")(0)
        ZObj.Definition = Defs(ZObj.Name)

        ZObj.Args = ZObj.Definition.Args.Clone
        Dim Args = Split(Split(Split(Macro, "(")(1), ")")(0), ",")

        If ZObj.Args.Count > 0 Then
            For i = 0 To Args.Count - 1
                ZObj.Args(i).Value = Args(i)
            Next
        End If
        Return ZObj
    End Function

    Public Shared Function FromDef(Def As ZDef, Args As IEnumerable(Of Object)) As Base
        Dim Obj As New ZBase
        ZType.FromMacro(Def.Macro, Args, Obj)

        Dim ZObj = BaseFromZBase(Obj)
        ZObj.Name = Def.Macro
        ZObj.Definition = Def
        ZObj.Args = Def.Args.Clone
        ZObj.Args.Base = ZObj
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

    Public Sub Update() Implements IBaseGeneralObject.Update
        Dim Obj As New ZBase
        ZType.FromMacro(_Name, Args.ToList().Select(Function(a) a.Value), Obj)
        FromStruct(Obj)
    End Sub

    Public Sub UpdatePosition(X As Double, Y As Double) Implements IBaseGeneralObject.UpdatePosition
        Args(0).Value = CInt(Math.Round(X))
        Args(1).Value = CInt(Math.Round(Y))
    End Sub

    Public Sub UpdatePosition(X As Double, Y As Double, W As Double, H As Double) Implements IBaseGeneralObject.UpdatePosition
        Args(0).Value = Math.Min(255, Math.Max(0, CInt(Math.Round(X))))
        Args(1).Value = Math.Min(255, Math.Max(0, CInt(Math.Round(Y))))
        Args(2).Value = Math.Min(255, Math.Max(0, CInt(Math.Round(W))))
        Args(3).Value = Math.Min(255, Math.Max(0, CInt(Math.Round(H))))
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
        IsInitializating = True
        Definition = Def
        _Name = Def.Macro
        Me.Args = Def.Args.Clone
        Me.Args.Base = Me
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
        IsInitializating = False
    End Sub

    Private _X As Byte
    Public Property X As Byte Implements IGeneralObject(Of ZBase).X
        Get
            Return _X
        End Get
        Set(value As Byte)
            If value <> _X Then
                _X = value
                RaisePropertyChanged("X")
            End If
        End Set
    End Property

    Private _Y As Byte
    Public Property Y As Byte Implements IGeneralObject(Of ZBase).Y
        Get
            Return _Y
        End Get
        Set(value As Byte)
            If value <> _Y Then
                _Y = value
                RaisePropertyChanged("Y")
            End If
        End Set
    End Property

    Private _Z As Byte
    Public Property Z As Byte Implements IGeneralObject(Of ZBase).Z
        Get
            Return _Z
        End Get
        Set(value As Byte)
            If value <> _Z Then
                _Z = value
                RaisePropertyChanged("Z")
            End If
        End Set
    End Property

    Private _W As Byte
    Public Property W As Byte Implements IGeneralObject(Of ZBase).W
        Get
            Return _W
        End Get
        Set(value As Byte)
            If value <> _W Then
                _W = value
                RaisePropertyChanged("W")
            End If
        End Set
    End Property

    Private _H As Byte
    Public Property H As Byte Implements IGeneralObject(Of ZBase).H
        Get
            Return _H
        End Get
        Set(value As Byte)
            If value <> _H Then
                _H = value
                RaisePropertyChanged("H")
            End If
        End Set
    End Property

    Private _D As Byte
    Public Property D As Byte Implements IGeneralObject(Of ZBase).D
        Get
            Return _D
        End Get
        Set(value As Byte)
            If value <> _D Then
                _D = value
                RaisePropertyChanged("D")
            End If
        End Set
    End Property

    Public ReadOnly Property Bounds As Rect Implements IGeneralObject(Of ZBase).Bounds
        Get
            Return New Rect(X, Y, W, H)
        End Get
    End Property

    Private _Image As Integer
    Public Property Image As Integer Implements IGeneralObject(Of ZBase).Image
        Get
            Return _Image
        End Get
        Set(value As Integer)
            If value <> _Image Then
                _Image = value
                RaisePropertyChanged("Image")
            End If
        End Set
    End Property

    Sub Move(Dx As Integer, Dy As Integer, Optional Dz As Integer = 0)
        X += Dx : Y += Dy : Z += Dz
    End Sub

    Public Function Clone() As Object Implements ICloneable.Clone
        Dim Copy As New Base
        Copy.IsInitializating = True
        Copy.Name = _Name
        Copy.Definition = Definition
        Copy.Args = New ArgsCollection(Copy)
        Args.ToList().ForEach(Sub(a) Copy.Args.Add(a.Clone))

        Copy.Image = Image
        Copy.NamedSlot = NamedSlot
        With Copy
            .X = X : .W = W
            .Y = Y : .H = H
            .Z = Z : .D = D
        End With
        Copy.IsInitializating = False
        Return Copy
    End Function

    Private Sub RaisePropertyChanged(PropName As String)
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(PropName))
    End Sub

    Public Event PropertyChanged(sender As Object, e As System.ComponentModel.PropertyChangedEventArgs) Implements System.ComponentModel.INotifyPropertyChanged.PropertyChanged

    Public Property IsInitializating As Boolean Implements IBaseGeneralObject.IsInitializating
End Class

#Region "Args collection and types"
Public Class ArgsCollection
    Inherits ObservableCollection(Of ZDefArg)
    Implements ICloneable

    Private _Base As IBaseGeneralObject
    Public Property Base As IBaseGeneralObject
        Get
            Return _Base
        End Get
        Set(value As IBaseGeneralObject)
            _Base = value
            Me.ToList().ForEach(Sub(m) m.Base = value)
        End Set
    End Property

    Public Sub New()
    End Sub

    Public Sub New(Base As IBaseGeneralObject)
        Me.Base = Base
    End Sub

    Protected Overrides Sub InsertItem(Index As Integer, Item As ZDefArg)
        If TypeOf Item Is ZDefArg Then
            DirectCast(Item, ZDefArg).Base = Base
        End If
        MyBase.InsertItem(Index, Item)

        ' Renumber all of the args
        For i = 0 To MyBase.Count - 1
            MyBase.Item(i).ArgIndex = i
        Next
    End Sub

    Public Function Clone() As Object Implements System.ICloneable.Clone
        Dim Copy As New ArgsCollection(Base)
        For Each Elem In Me
            Copy.Add(Elem.Clone)
        Next
        Return Copy
    End Function
End Class

Public Class ZDefArgGenState
    Inherits ZDefArg

    Public Sub New()
    End Sub

    Public Sub New(Name As String, Description As String)
        MyBase.New(Name, Description)
    End Sub

    Public Overrides Function Clone() As Object
        Dim Copy As New ZDefArgGenState(Name, Description)
        Copy.Value = Value
        Return Copy
    End Function
End Class

Public Class ZDefArgBoolean
    Inherits ZDefArg

    Public Sub New()
    End Sub

    Public Sub New(Name As String, Description As String)
        MyBase.New(Name, Description)
    End Sub

    Public Overrides Function Clone() As Object
        Dim Copy As New ZDefArgBoolean(Name, Description)
        Copy.Value = Value
        Return Copy
    End Function
End Class

Public Class ZDefArgSlot
    Inherits ZDefArg

    Private _Scenario As Scenario
    Public Property Slots As ICollection(Of String)

    Public Sub New()
    End Sub

    Public Sub New(Name As String, Description As String, Scenario As Scenario)
        MyBase.New(Name, Description)
        _Scenario = Scenario
        Slots = Scenario.NamedSlots
    End Sub

    Public Overrides Function Clone() As Object
        Dim Copy As New ZDefArgSlot(Name, Description, _Scenario)
        Copy.Value = Value
        Copy.Slots = Slots
        Return Copy
    End Function
End Class


Public Class ZDefArgGraphic
    Inherits ZDefArg

    Public Sub New()
    End Sub

    Public Property Graphics As IEnumerable(Of ZeldaImage)

    Public Sub New(Name As String, Description As String, Graphics As IEnumerable(Of ZeldaImage))
        MyBase.New(Name, Description)
        Me.Graphics = Graphics
    End Sub

    Public Overrides Function Clone() As Object
        Dim Copy As New ZDefArgGraphic(Name, Description, Graphics)
        Copy.Value = Value
        Copy.Graphics = Graphics
        Return Copy
    End Function
End Class

Public Class ZDefArgObjectID
    Inherits ZDefArg
    Public Sub New()
    End Sub

    Public Property ObjectIDs As IEnumerable(Of String)

    Public Sub New(Name As String, Description As String)
        MyBase.New(Name, Description)
        Dim ObjectIDs As New List(Of String)

        For Each Label In SPASMHelper.Labels
            If Char.ToUpper(Label.Key.Chars(0)) = "K" And Label.Value < &H4000 And Not Label.Key.Contains("_GFX") Then
                ObjectIDs.Add(Label.Key)
            End If
        Next

        ObjectIDs.Sort()
        Me.ObjectIDs = ObjectIDs
    End Sub

    Public Overrides Function Clone() As Object
        Dim Copy As New ZDefArgObjectID(Name, Description)
        Copy.Value = Value
        Copy.ObjectIDs = ObjectIDs
        Return Copy
    End Function
End Class

Public Class ZDefArgObjectFlags
    Inherits ZDefArg

    Public Sub New()
    End Sub

    Public Sub New(Name As String, Description As String)
        MyBase.New(Name, Description)
        Me.Value = "0"
    End Sub

    Public Overrides Function Clone() As Object
        Dim Copy As New ZDefArgObjectFlags(Name, Description)
        Copy.Value = Value
        Return Copy
    End Function
End Class

Public Class ZDefArgEnemyFlags
    Inherits ZDefArg

    Public Property EnemyTypes As ICollection(Of String)

    Public Sub New()
    End Sub

    Public Sub New(Name As String, Description As String)
        MyBase.New(Name, Description)

        Dim EnemyTypes = From a In SPASMHelper.Labels
                         Where a.Key.ToUpper.StartsWith("ET_")
                         Order By a.Value
                         Select a.Key

        Me.EnemyTypes = EnemyTypes.ToList()
    End Sub

    Public Overrides Function Clone() As Object
        Dim Copy As New ZDefArgEnemyFlags(Name, Description)
        Copy.Value = Value
        Return Copy
    End Function
End Class

Public Class ZDefArg8Bit
    Inherits ZDefArg

    Public Sub New()
    End Sub

    Public Sub New(Name As String, Description As String)
        MyBase.New(Name, Description)
    End Sub

    Public Overrides Function Clone() As Object
        Dim Copy As New ZDefArg8Bit(Name, Description)
        Copy.Value = Value
        Return Copy
    End Function
End Class

Public Class ZDefArg16Bit
    Inherits ZDefArg

    Public Sub New()
    End Sub

    Public Sub New(Name As String, Description As String)
        MyBase.New(Name, Description)
    End Sub

    Public Overrides Function Clone() As Object
        Dim Copy As New ZDefArg16Bit(Name, Description)
        Copy.Value = Value
        Return Copy
    End Function
End Class

Public Class ZDefArg
    Implements ICloneable

    Public Property Base As IBaseGeneralObject

    Public Property IsOptional As Boolean = False

    Public Property Name As String
    Public Property Description As String

    Public Property ArgIndex As Integer

    Public ReadOnly Property LabelText
        Get
            Return If(Description.EndsWith("?"), Description, Description & ":")
        End Get
    End Property

    Private _Value As String
    Public Property Value As String
        Get
            Return _Value
        End Get
        Set(value As String)
            value = Trim(value)
            If value = "" Then value = Nothing
            If _Value <> value Then
                _Value = value
                If Base IsNot Nothing AndAlso Not Base.IsInitializating Then
                    Base.Update()
                End If
            End If
        End Set
    End Property

    Public Sub New(Name As String, Description As String)
        Me.Name = Name
        Me.Description = Description
    End Sub

    Public Sub New()
    End Sub

    Public Overridable Function Clone() As Object Implements System.ICloneable.Clone
        Dim Copy As New ZDefArg(Name, Description)
        Copy.Base = Base
        Copy.Value = Value
        Return Copy
    End Function
End Class
#End Region

Public Class ArgNameAndIndex
    Public Property ArgIndex As Integer
    Public Property Name As String

    Public Sub New(Index As Integer, Name As String)
        Me.ArgIndex = Index
        Me.Name = Name
    End Sub
End Class

<Serializable()>
Public Class ZDef
    Public Property Name As String
    Public Property Macro As String
    Public Property Description As String
    Public Property Args As ArgsCollection
    Public Property DefaultImage As Integer

    Public Property DefaultW As Byte
    Public Property DefaultH As Byte

    Public Property DefaultZ As Byte

    Public ReadOnly Property ArgsAndLabels As ICollection(Of Object)
        Get
            Dim Indexes = Enumerable.Range(0, Args.Count)
            Return Args.Cast(Of Object).Concat(Indexes.Select(Function(i) New ArgNameAndIndex(i, Args(i).LabelText))).ToList()
        End Get
    End Property

    Public Sub New()
    End Sub

    Public Sub New(Name As String, Macro As String, Description As String, ObjType As Type)
        Me.Name = Name
        Me.Macro = Macro
        Me.Description = Description
        Args = New ArgsCollection(Nothing)

        Dim M = ObjType.BaseType.GetMethod("FromMacro")

        If ObjType <> GetType(ZMisc) Then
            Dim Defs As New Dictionary(Of String, ZDef)
            Defs.Add(Macro, Me)
            Dim ObjectInstance = M.Invoke(Nothing, {Defs, Macro & "(0, 0)"})

            DefaultImage = ObjectInstance.Image
            DefaultW = ObjectInstance.W
            DefaultH = ObjectInstance.H
            DefaultZ = ObjectInstance.Z
        Else
            DefaultImage = 0
            DefaultW = 16
            DefaultH = 16
            DefaultZ = 0
        End If
    End Sub

    Public Sub AddArg(Name As String, Description As String, Scenario As Scenario, Optional IsOptional As Boolean = False)
        Dim NewArg As ZDefArg
        If Description.StartsWith("Is ") Or Description.StartsWith("Are ") Then
            NewArg = New ZDefArgBoolean(Name, Description)
        Else
            Select Case Name.ToLower
                Case "x", "y", "z", "w", "h", "d", "ac", "cc"
                    NewArg = New ZDefArg8Bit(Name, Description)
                Case "cp", "ax", "health", "a", "code", "aux"
                    NewArg = New ZDefArg16Bit(Name, Description)
                Case "of"
                    NewArg = New ZDefArgObjectFlags(Name, Description)
                Case "ef"
                    NewArg = New ZDefArgEnemyFlags(Name, Description)
                Case "type", "ztype"
                    NewArg = New ZDefArgObjectID(Name, Description)
                Case "ap"
                    NewArg = New ZDefArgGraphic(Name, Description, Scenario.Images)
                Case "g", "gg"
                    NewArg = New ZDefArgGenState(Name, Description)
                Case "slot"
                    NewArg = New ZDefArgSlot(Name, Description, Scenario)
                Case Else
                    NewArg = New ZDefArg(Name, Description)
            End Select
        End If
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
        Dim ArgsToTrim = 0
        For i = Args.Count - 1 To 0 Step -1
            If Args(i) = Nothing Or (TypeOf Args(i) is String andalso Args(i) = "") Then
                ArgsToTrim += 1
            Else
                Exit For
            End If
        Next

        Dim TrimmedArgs = Args.Take(Args.Count - ArgsToTrim)

        Dim Macro As String = " " & Name & "("
        For i = 0 To TrimmedArgs.Count - 1
            If TrimmedArgs(i) Is Nothing Then
                i = i + 1
                Macro = Left(Macro, Len(Macro) - 1)
            Else
                Macro &= TrimmedArgs(i).ToString()
            End If
            If i < TrimmedArgs.Count - 1 Then
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

    Public Property ExtractOnCount As ExtractOnCount

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