package net.lecousin.framework.io;

import java.io.IOException;
import java.io.InputStream;

public class BufferedInputStream_Threaded extends InputStream {

	public BufferedInputStream_Threaded(InputStream stream, int buffer_size, int nb_buffers) {
		this.stream = stream;
		this.buffer_max_size = buffer_size;
		this.buffers = new byte[nb_buffers][buffer_size];
		this.buffer_size = new int[nb_buffers];
		read_buffer = nb_buffers-1;
		read_buffer_pos = this.buffer_size[read_buffer];
		next_buffer_to_fill = 0;
		// make it able to start right away
		try {
			this.buffer_size[0] = stream.read(this.buffers[0], 0, buffer_size > 4096 ? 4096 : buffer_size);
			next_buffer_to_fill++;
			t = new T();
			t.start();
		} catch (IOException e) {
			ex = e;
			eof = true;
		}
	}
	
	private InputStream stream;
	private byte[][] buffers;
	private int[] buffer_size;
	private int buffer_max_size;
	private boolean eof = false;
	private IOException ex = null;
	private T t;
	private boolean closed = false;
	private Object sync = new Object() {};
	private int read_buffer;
	private int read_buffer_pos;
	private int next_buffer_to_fill;
	
	private class T extends Thread {
		@Override
		public void run() {
			boolean end = false;
			int nb, nb2;
			do {
				synchronized (sync) {
					if (next_buffer_to_fill == read_buffer) {
						try { sync.wait(); } catch (InterruptedException e) { break; }
						continue;
					}
				}
				try {
					nb = stream.read(buffers[next_buffer_to_fill]);
					if (nb < 0) break;
					if (nb < buffer_max_size) {
						// try another one
						nb2 = stream.read(buffers[next_buffer_to_fill], nb, buffer_max_size-nb);
						if (nb2 < 0) {
							end = true;
						} else {
							nb += nb2;
						}
					}
					buffer_size[next_buffer_to_fill] = nb;
					synchronized (sync) {
						if (++next_buffer_to_fill == buffers.length) next_buffer_to_fill = 0;
						sync.notify();
					}
					if (end) break;
				} catch (IOException e) {
					ex = e;
					break;
				}
			} while(!closed);
			eof = true;
			synchronized (sync) { sync.notify(); }
		}
	}
	
	@Override
	public void close() throws IOException {
		closed = true;
		eof = true;
		synchronized (sync) { sync.notifyAll(); }
		stream.close();
	}
	
	@Override
	public int read(byte[] b, int off, int len) throws IOException {
		if (read_buffer_pos == buffer_size[read_buffer]) {
			int next = read_buffer+1;
			if (next == buffers.length) next = 0;
			while (true) {
				if (next == next_buffer_to_fill) {
					// we need to wait for the filler
					// but it may have been filled while checking (no mutual exclusion to improve perf the first time)
					synchronized (sync) {
						if (next == next_buffer_to_fill) {
							if (ex != null) throw ex;
							if (eof) return -1;
							long start = System.nanoTime();
							try { sync.wait(); } catch (InterruptedException e) { return -1; }
							waited += System.nanoTime()-start;
							continue;
						}
					}
				}
				break;
			}
			synchronized (sync) {
				read_buffer = next;
				read_buffer_pos = 0;
				sync.notify();
			}
		}
		int l = len;
		if (read_buffer_pos+l > buffer_size[read_buffer]) l = buffer_size[read_buffer]-read_buffer_pos;
		System.arraycopy(buffers[read_buffer], read_buffer_pos, b, off, l);
		read_buffer_pos += l;
		if (l == len) return len;
		return l+read(b,off+l,len-l);
	}

	@Override
	public int read() throws IOException {
		if (read_buffer_pos == buffer_size[read_buffer]) {
			int next = read_buffer+1;
			if (next == buffers.length) next = 0;
			while (true) {
				if (next == next_buffer_to_fill) {
					// we need to wait for the filler
					// but it may have been filled while checking (no mutual exclusion to improve perf the first time)
					synchronized (sync) {
						if (next == next_buffer_to_fill) {
							if (ex != null) throw ex;
							if (eof) return -1;
							long start = System.nanoTime();
							try { sync.wait(); } catch (InterruptedException e) { return -1; }
							waited += System.nanoTime()-start;
							continue;
						}
					}
				}
				break;
			}
			synchronized (sync) {
				read_buffer = next;
				read_buffer_pos = 0;
				sync.notify();
			}
		}
		return buffers[read_buffer][read_buffer_pos++] & 0xFF;
	}

	public int readUTF16LE() throws IOException {
		if (read_buffer_pos == buffer_size[read_buffer]) {
			int next = read_buffer+1;
			if (next == buffers.length) next = 0;
			while (true) {
				if (next == next_buffer_to_fill) {
					// we need to wait for the filler
					// but it may have been filled while checking (no mutual exclusion to improve perf the first time)
					synchronized (sync) {
						if (next == next_buffer_to_fill) {
							if (ex != null) throw ex;
							if (eof) return -1;
							long start = System.nanoTime();
							try { sync.wait(); } catch (InterruptedException e) { return -1; }
							waited += System.nanoTime()-start;
							continue;
						}
					}
				}
				break;
			}
			synchronized (sync) {
				read_buffer = next;
				read_buffer_pos = 0;
				sync.notify();
			}
		}
		int i = buffers[read_buffer][read_buffer_pos++] & 0xFF;
		if (read_buffer_pos == buffer_size[read_buffer]) {
			int next = read_buffer+1;
			if (next == buffers.length) next = 0;
			while (true) {
				if (next == next_buffer_to_fill) {
					// we need to wait for the filler
					// but it may have been filled while checking (no mutual exclusion to improve perf the first time)
					synchronized (sync) {
						if (next == next_buffer_to_fill) {
							if (ex != null) throw ex;
							if (eof) return -1;
							long start = System.nanoTime();
							try { sync.wait(); } catch (InterruptedException e) { return -1; }
							waited += System.nanoTime()-start;
							continue;
						}
					}
				}
				break;
			}
			synchronized (sync) {
				read_buffer = next;
				read_buffer_pos = 0;
				sync.notify();
			}
		}
		return i | ((buffers[read_buffer][read_buffer_pos++] & 0xFF) << 8);
	}
	
	private long waited = 0;
	
	public long getWaitedNanosecondsForDisk() { return waited; }
	
}
