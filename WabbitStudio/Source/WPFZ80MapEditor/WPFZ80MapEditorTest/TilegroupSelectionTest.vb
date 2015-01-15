Imports System.Text
Imports Microsoft.VisualStudio.TestTools.UnitTesting
Imports WPFZ80MapEditor
Imports System.Windows

<TestClass()> Public Class TilegroupSelectionTest

    <TestMethod()> Public Sub SingleTile()
        Dim Sel = New TilegroupSelection(Nothing, {New TilegroupEntry(0, 0)})

        Assert.IsTrue(Sel.Vertices.Contains(New Point(0, 0)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(0, 16)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(16, 0)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(16, 16)))

    End Sub
    <TestMethod()> Public Sub SingleTileShifted()
        Dim Sel = New TilegroupSelection(Nothing, {New TilegroupEntry(16 + 1, 0)})

        Assert.IsTrue(Sel.Vertices.Contains(New Point(0, 0)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(0, 16)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(16, 0)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(16, 16)))

    End Sub

    <TestMethod()> Public Sub DoubleTile()
        Dim Sel = New TilegroupSelection(Nothing, {
                                         New TilegroupEntry(16 + 1, 0),
                                         New TilegroupEntry(16 + 2, 0)})

        Assert.IsTrue(Sel.Vertices.Contains(New Point(0, 0)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(0, 16)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(16, 0)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(16, 16)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(32, 16)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(32, 0)))

        Assert.AreEqual(6, Sel.Vertices.Count)

    End Sub

    <TestMethod()> Public Sub TripleTile()
        Dim Sel = New TilegroupSelection(Nothing, {
                                         New TilegroupEntry(16 + 1, 0),
                                         New TilegroupEntry(32 + 1, 0),
                                         New TilegroupEntry(16 + 2, 0)})

        Assert.IsTrue(Sel.Vertices.Contains(New Point(0, 0)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(0, 16)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(16, 0)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(16, 16)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(32, 16)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(32, 0)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(0, 32)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(16, 32)))

        Assert.AreEqual(8, Sel.Vertices.Count)

    End Sub


    <TestMethod()> Public Sub QuadTile()
        Dim Sel = New TilegroupSelection(Nothing, {
                                         New TilegroupEntry(117, 0),
                                         New TilegroupEntry(118, 0),
                                         New TilegroupEntry(134, 0),
                                         New TilegroupEntry(135, 0)})

        Assert.IsTrue(Sel.Vertices.Contains(New Point(0, 0)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(0, 16)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(16, 0)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(16, 16)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(32, 0)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(32, 16)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(16, 32)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(32, 32)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(48, 16)))
        Assert.IsTrue(Sel.Vertices.Contains(New Point(48, 32)))

        Assert.AreEqual(10, Sel.Vertices.Count)

    End Sub

End Class