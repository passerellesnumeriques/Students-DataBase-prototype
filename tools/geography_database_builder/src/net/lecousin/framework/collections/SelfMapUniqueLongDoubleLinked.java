package net.lecousin.framework.collections;

import java.io.Externalizable;
import java.io.IOException;
import java.io.ObjectInput;
import java.io.ObjectOutput;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;

public class SelfMapUniqueLongDoubleLinked<EntryType extends SelfMap.Entry<Long>> implements SelfMap<Long,EntryType>, Externalizable {

	private static class Element<T> {
		Element(T e, Element<T> p, Element<T> n) { element = e; prev = p; next = n; }
		T element;
		Element<T> next;
		Element<T> prev;
	}

	@SuppressWarnings("unchecked")
	public SelfMapUniqueLongDoubleLinked(int nbBuckets) {
		buckets_head = new Element[nbBuckets];
		buckets_tail = new Element[nbBuckets];
	}
	public SelfMapUniqueLongDoubleLinked() { this(20); }

	private Element<EntryType>[] buckets_head, buckets_tail;
	private int size = 0;
	
	private static final int bankSize = 200;
	@SuppressWarnings("rawtypes")
	private static Element[] bankOfElements = new Element[bankSize];
	private static int indexBank = -1;
	
	@SuppressWarnings("unchecked")
	private static synchronized <T> Element<T> createElement(T e, Element<T> p, Element<T> n) {
		if (indexBank < 0) return new Element<T>(e, p, n);
		Element<T> elt = bankOfElements[indexBank--];
		elt.element = e;
		elt.next = n;
		elt.prev = p;
		return elt;
	}
	private static synchronized <T> void free(Element<T> e) {
		if (indexBank == bankSize-1) return;
		bankOfElements[++indexBank] = e;
		e.element = null;
		e.next = null;
		e.prev = null;
	}

	private Element<EntryType> getBucketHead(long id) {
		int hc = (int)(id % buckets_head.length);
		return buckets_head[hc];
	}
	private Element<EntryType> getBucketTail(long id) {
		int hc = (int)(id % buckets_tail.length);
		return buckets_head[hc];
	}

	@Override
	public void put(EntryType entry) {
		size++;
		long id = entry.getHashObject();
		int hc = (int)(id % buckets_head.length);
		Element<EntryType> first = buckets_head[hc];
		if (first == null) {
			buckets_head[hc] = buckets_tail[hc] = createElement(entry, null, null);
			return;
		}
		long fid = first.element.getHashObject();
		if (fid > id) {
			buckets_head[hc] = first.prev = createElement(entry, null, first);
			return;
		}
		if (first.next == null) {
			buckets_tail[hc] = first.next = createElement(entry, first, null);
			return;
		}
		Element<EntryType> last = buckets_tail[hc];
		long lid = last.element.getHashObject();
		if (lid < id) {
			buckets_tail[hc].next = createElement(entry, buckets_tail[hc], null);
			buckets_tail[hc] = buckets_tail[hc].next;
			return;
		}
		if (id-fid < lid-id) {
			do {
				fid = first.next.element.getHashObject();
				if (fid > id) {
					first.next.prev = createElement(entry, first, first.next);
					first.next = first.next.prev;
					return;
				}
				first = first.next;
			} while (first.next != null);
			buckets_tail[hc] = first.next = createElement(entry, first, null);
		} else {
			do {
				lid = last.prev.element.getHashObject();
				if (lid < id) {
					last.prev.next = createElement(entry, last.prev, last);
					last.prev = last.prev.next;
					return;
				}
				last = last.prev;
			} while (last.prev != null);
			buckets_head[hc] = last.prev = createElement(entry, null, last);
		}
	}
	/** To be used only if we are sure the key is bigger than any other key in the map */
	public void putLast(EntryType entry) {
		size++;
		long id = entry.getHashObject();
		int hc = (int)(id % buckets_head.length);
		Element<EntryType> last = buckets_tail[hc];
		if (last == null) {
			buckets_head[hc] = buckets_tail[hc] = createElement(entry, null, null);
			return;
		}
		last.next = createElement(entry, last, null);
		buckets_tail[hc] = last.next;
	}
	@Override
	public boolean add(EntryType entry) { put(entry); return true; }
	@Override
	public boolean addAll(Collection<? extends EntryType> entries) {
		for (EntryType e : entries)
			put(e);
		return true;
	}
	@Override
	public void clear() {
		for (int i = 0; i < buckets_head.length; ++i)
			buckets_head[i] = buckets_tail[i] = null;
		size = 0;
	}

	@Override
	public EntryType removeKey(Long id) {
		return removeKey((long)id);
	}
	public EntryType removeKey(long id) {
		int hc = (int)(id % buckets_head.length);
		Element<EntryType> first = buckets_head[hc];
		if (first == null) return null;
		long fid = first.element.getHashObject();
		if (fid == id) {
			buckets_head[hc] = first.next;
			if (first.next == null) buckets_tail[hc] = null; else first.next.prev = null;
			EntryType result = first.element;
			free(first);
			size--;
			return result;
		}
		if (fid > id) return null;
		Element<EntryType> last = buckets_tail[hc];
		long lid;
		if (last == first)
			lid = fid;
		else {
			lid = last.element.getHashObject();
			if (lid == id) {
				buckets_tail[hc] = last.prev;
				buckets_tail[hc].next = null;
				EntryType result = last.element;
				free(last);
				size--;
				return result;
			}
		}
		if (lid < id) return null;
		if (id-fid < lid-id) {
			if (first.next == null) return null;
			do {
				EntryType e = first.next.element;
				fid = e.getHashObject();
				if (fid == id) {
					Element<EntryType> next = first.next;
					first.next.next.prev = first;
					first.next = first.next.next;
					free(next);
					size--;
					return e;
				}
				if (fid > id) return null;
				first = first.next;
			} while (first.next != null);
		} else {
			if (last.prev == null) return null;
			do {
				EntryType e = last.prev.element;
				lid = e.getHashObject();
				if (lid == id) {
					Element<EntryType> prev = last.prev;
					last.prev.prev.next = last;
					last.prev = last.prev.prev;
					free(prev);
					size--;
					return e;
				}
				if (lid < id) return null;
				last = last.prev;
			} while (last.prev != null);
		}
		return null;
	}

	@Override
	public EntryType removeEntry(EntryType entry) {
		return removeKey((long)entry.getHashObject());
	}
	
	public EntryType removeFirst() {
		if (size == 0) return null;
		for (int i = 0; i < buckets_head.length; ++i) {
			Element<EntryType> ptr = buckets_head[i];
			if (ptr == null) continue;
			if ((buckets_head[i] = ptr.next) == null) buckets_tail[i] = null;
			EntryType entry = ptr.element;
			free(ptr);
			size--;
			return entry;
		}
		return null;
	}

	@Override
	@SuppressWarnings("unchecked")
	public boolean remove(Object o)	{ 
		return removeEntry((EntryType)o) != null;
	}

	@Override
	@SuppressWarnings("unchecked")
	public boolean removeAll(Collection<?> c) {
		boolean result = false;
		for (EntryType e : (Iterable<? extends EntryType>)c) {
			if (removeEntry(e) != null) result = true;
		}
		return result;
	}

	@Override
	public EntryType get(Long id) {
		return get((long)id);
	}
	public EntryType get(long id) {
		Element<EntryType> first = getBucketHead(id);
		if (first == null) return null;
		long fid = first.element.getHashObject();
		if (fid == id) return first.element;
		if (fid > id) return null;
		if (first.next == null) return null;
		Element<EntryType> last = getBucketTail(id);
		long lid = last.element.getHashObject();
		if (lid == id) return last.element;
		if (lid < id) return null;
		if (id-fid < lid-id) {
			first = first.next;
			do {
				fid = first.element.getHashObject();
				if (fid == id) return first.element;
				if (fid > id) return null;
				first = first.next;
			} while (first != null);
		} else {
			last = last.prev;
			do {
				lid = last.element.getHashObject();
				if (lid == id) return last.element;
				if (lid < id) return null;
				last = last.prev;
			} while (last != null);
		}
		return null;
	}

	@Override
	public boolean containsKey(Long id) {
		return containsKey((long)id);
	}
	public boolean containsKey(long id) {
		Element<EntryType> first = getBucketHead(id);
		if (first == null) return false;
		long fid = first.element.getHashObject();
		if (fid == id) return true;
		if (fid > id) return false;
		if (first.next == null) return false;
		Element<EntryType> last = getBucketTail(id);
		long lid = last.element.getHashObject();
		if (lid == id) return true;
		if (lid < id) return false;
		if (id-fid < lid-id) {
			first = first.next;
			do {
				fid = first.element.getHashObject();
				if (fid == id) return true;
				if (fid > id) return false;
				first = first.next;
			} while (first != null);
		} else {
			last = last.prev;
			do {
				lid = last.element.getHashObject();
				if (lid == id) return true;
				if (lid < id) return false;
				last = last.prev;
			} while (last != null);
		}
		return false;
	}

	@Override
	public boolean containsEntry(EntryType entry) { return containsKey(entry.getHashObject()); }
	@Override
	@SuppressWarnings("unchecked")
	public boolean contains(Object o) { return containsEntry((EntryType)o); }

	@Override
	@SuppressWarnings("unchecked")
	public boolean containsAll(Collection<?> entries) {
		for (EntryType e : (Iterable<? extends EntryType>)entries)
			if (!containsEntry(e)) return false;
		return true;
	}

	@Override
	public boolean retainAll(Collection<?> c) {
		boolean result = false;
		for (int iBucket = 0; iBucket < buckets_head.length; ++iBucket) {
			Element<EntryType> ptr = buckets_head[iBucket];
			if (ptr == null) continue;
			do {
				if (!c.contains(ptr.element)) {
					Element<EntryType> p = ptr;
					ptr = buckets_head[iBucket] = ptr.next;
					ptr.prev = null;
					free(p);
				} else break;
			} while (ptr != null);
			if (ptr == null) { buckets_tail[iBucket] = null; continue; }
			result = true;
			if (ptr.next == null) continue;
			do {
				if (!c.contains(ptr.next.element)) {
					Element<EntryType> e = ptr.next;
					if ((ptr.next = e.next) != null)
						ptr.next.prev = ptr;
					else
						buckets_tail[iBucket] = ptr;
					free(e);
				} else
					ptr = ptr.next;
			} while (ptr.next  != null);
		}
		return result;
	}

	@Override
	public int size() { return size; }
	@Override
	public boolean isEmpty(){ return size == 0; }

	@Override
	public Object[] toArray() {
		Object[] result = new Object[size()];
		fillArray(result);
		return result;
	}
	@SuppressWarnings("unchecked")
	@Override
	public <T> T[] toArray(T[] a) {
		if (a.length < size())
			a = (T[])java.lang.reflect.Array.newInstance(
					a.getClass().getComponentType(), size());
		fillArray(a);
		return a;
	}
	private void fillArray(Object[] result) {
		int pos = 0;
		for (int i = 0; i < buckets_head.length; ++i) {
			Element<EntryType> ptr = buckets_head[i];
			while (ptr != null) {
				result[pos++] = ptr.element;
				ptr = ptr.next;
			}
		}
	}
	@Override
	public List<EntryType> asList() {
		ArrayList<EntryType> list = new ArrayList<EntryType>(size());
		for (int i = 0; i < buckets_head.length; ++i) {
			Element<EntryType> ptr = buckets_head[i];
			while (ptr != null) {
				list.add(ptr.element);
				ptr = ptr.next;
			}
		}
		return list;
	}
	
	@Override
	public Iterator<EntryType> iterator() { return new SelfMapIterator(); }

	private class SelfMapIterator implements Iterator<EntryType> {
		SelfMapIterator() {
			iBucket = 0;
			while ((ptr = buckets_head[iBucket]) == null) if (++iBucket >= buckets_head.length) break;
		}
		int iBucket;
		Element<EntryType> ptr;
		
		Element<EntryType> last = null;
		int last_bucket = -1;

		@Override
		public boolean hasNext() { return ptr != null; }
		@Override
		public EntryType next() {
			last = ptr; last_bucket = iBucket;
			EntryType e = ptr.element;
			if ((ptr = ptr.next) != null) return e;
			if (++iBucket >= buckets_head.length) return e;
			while ((ptr = buckets_head[iBucket]) == null) if (++iBucket >= buckets_head.length) break;
			return e;
		}
		
		@Override
		public void remove() {
			if (last.prev == null)
				buckets_head[last_bucket] = last.next;
			else
				last.prev.next = last.next;
			if (last.next == null)
				buckets_tail[last_bucket] = last.prev;
			else
				last.next.prev = last.prev;
		}
	}
	
	@Override
	public void writeExternal(ObjectOutput out) throws IOException {
		out.writeInt(buckets_head.length);
		out.writeInt(size);
		for (int i = 0; i < buckets_head.length; ++i) {
			Element<EntryType> e = buckets_head[i];
			while (e != null) {
				out.writeObject(e.element);
				e = e.next;
			}
		}
	}
	@Override
	@SuppressWarnings("unchecked")
	public void readExternal(ObjectInput in) throws IOException, ClassNotFoundException {
		size = in.readInt();
		buckets_head = new Element[size];
		buckets_tail = new Element[size];
		size = in.readInt();
		for (int i = 0; i < size; ++i)
			add((EntryType)in.readObject());
	}
}
