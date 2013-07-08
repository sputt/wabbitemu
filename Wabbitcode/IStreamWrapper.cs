#if USE_DLL

using SPASM;
using System.IO;

public class CStreamWrapper : Stream
{
private ulong m_nPos;
private IStream m_pStream;

public CStreamWrapper(IStream pStream)
    {
        m_pStream = pStream;
        m_nPos = 0;
        Seek(0, SeekOrigin.Begin);
    }

    public override bool CanRead
    {
        get
        {
            return true;
        }
    }

    public override bool CanSeek
    {
        get
        {
            return true;
        }
    }

    public override bool CanWrite
    {
        get
        {
            return false;
        }
    }

    public override long Length
    {
        get
        {
            tagSTATSTG statstg;
            m_pStream.Stat(out statstg, 0);
            return (long) statstg.cbSize.QuadPart;
        }
    }

    public override long Position
    {
        get
        {
            return (long) m_nPos;
        }
        set
        {
            Position = (long) m_nPos;
        }
    }

    public override void Flush()
    {
        m_pStream.Commit(0);
    }

    public override int Read(byte[] buffer, int offset, int count)
    {
        uint pcbRead = 0;
        Seek((long) m_nPos, SeekOrigin.Begin);
        m_pStream.RemoteRead(out buffer[offset], (uint) 1, out pcbRead);
        Seek(pcbRead, SeekOrigin.Current);
        return (int) pcbRead;
    }

    public override long Seek(long offset, SeekOrigin origin)
    {
        _LARGE_INTEGER nSeek;
        _ULARGE_INTEGER curPos;

        nSeek.QuadPart = offset;
        curPos.QuadPart = 0;

        uint dwOrigin;
        switch (origin)
        {
        case SeekOrigin.Begin:
            dwOrigin = 0;
            m_nPos = (ulong) offset;
            break;
        case SeekOrigin.Current:
            dwOrigin = 0;
            m_nPos = (ulong) (((long) m_nPos) + offset);
            offset = (long) m_nPos;
            break;
        case SeekOrigin.End:
            dwOrigin = 2;
            m_nPos = (ulong) (Length + offset);
            break;
        default:
            throw new System.ArgumentException();
        }
        m_pStream.RemoteSeek(nSeek, dwOrigin, out curPos);
        return (long) curPos.QuadPart;
    }

    public override void SetLength(long value)
    {
        _ULARGE_INTEGER largeLength;
        largeLength.QuadPart = (ulong) value;
        m_pStream.SetSize(largeLength);
    }

    public override void Write(byte[] buffer, int offset, int count)
    {
        throw new System.NotImplementedException();
    }
}

#endif