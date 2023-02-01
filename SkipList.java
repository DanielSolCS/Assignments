import java.util.*;

class Node<T extends Comparable<T>> implements Comparable<Node<T>>
{
	private T data;
	private int height;
	// list of pointers at different heights
	private ArrayList<Node<T>> nextptr = new ArrayList<Node<T>>();
	
	Node(int height)
	{
		this.height = height;
		for (int x = 0; x < this.height; x++)
		{
			this.nextptr.add(null);
		}
	}
	
	Node(T data, int height)
	{
		this.height = height;
		this.data = data;
		for (int x = 0; x < this.height; x++)
		{
			this.nextptr.add(null);
		}
	}
	
	public T value()
	{
		return this.data;
	}
	
	public int height()
	{
		return this.height;
	}
	
	// returns the next node at given level
	public Node<T> next(int level)
	{
		if (level >= 0 && level < this.height)
			return this.nextptr.get(level);
		return null;
	}
	
	// sets current node's pointer at the given level to given node
	public void setNext(int level, Node<T> node)
	{
		this.nextptr.set(level, node);
	}
	
	// raises current node's height by one
	public void grow()
	{
		this.nextptr.add(null);
		this.height++;
	}
	
	// 50% chance of raising current node's height by one
	public boolean maybeGrow()
	{
		Random rand = new Random();
		
		if (rand.nextInt(2) == 1)
		{
			this.nextptr.add(null);
			this.height++;
			return true;
		}
		return false;
	}
	
	// lowers current node's height by one
	public void trim(int height)
	{
		while (this.height > height)
		{
			this.nextptr.remove(this.height - 1);
			this.height--;
		}
	}
	
	// returns this.data - x.data
	public int compareTo(Node<T> x)
	{
		return this.data.compareTo(x.data);
	}
	
}

public class SkipList<N extends Comparable<N>>
{
	private Node<N> head;
	private int height;
	private int size;
	
	SkipList()
	{
		this.height = 1;
		this.head = new Node<N>(this.height);
		this.size = 0;
	}
	
	SkipList(int height)
	{
		this.height = height;
		this.head = new Node<N>(this.height);
		this.size = 0;
	}
	
	public int size()
	{
		return size;
	}
	
	public int height()
	{
		return height;
	}
	
	public Node<N> head()
	{
		return this.head;
	}
	
	// inserts node into skiplist, sorted
	public void insert(N data)
	{
		Random rand = new Random();
		// first node
		if (this.height == 1 && this.size == 0)
		{
			Node<N> newbie = new Node<N>(data, 1);
			this.head.setNext(0, newbie); 
			this.size++;
			return;
		}
		// first node with a set skiplist height
		else if (this.size == 0)
		{
			int counter = 1;
			// chooses a height, 1/2^n chance for n + 1 height
			while ((rand.nextInt(2) == 1) && (counter < this.height))
			{
				counter++;
			}
			
			Node<N> newbie = new Node<N>(data, counter);
			
			// head pointer to point to new node
			for (int i = counter - 1; i >= 0; i--)
			{
				this.head.setNext(i, newbie);
			}
			this.size++;
			return;
		}
		// for nodes 2 -> oo
		
		// dummy node to hold data value for comparisons
		Node<N> comp = new Node<N>(data, 0);
		// array to hold values of nodes that will have to point to new node
		ArrayList<Node<N>> temps = new ArrayList<Node<N>>(this.height);
		
		for (int x = 0; x < this.height; x++)
		{
			temps.add(null);
		}
		
		// keeps track of current level
		int level = this.height - 1;
		Node<N> temp = this.head;
		
		while (this.size > 0)
		{
			// next 5 lines are literally just to shorten line 187 below 100 char
			int comparison = 0;
			if (temp.next(level) != null)
			{
				comparison = temp.next(level).compareTo(comp);
			}
			// if next move would skip the proper position of new node
			if (temp.next(level) == null || comparison > 0 || comparison == 0)
			{
				// if at lowest level, we've arrived to insert point
				if (level == 0)
				{
					temps.set(level, temp);
					break;
				}
				// mark nodes that has to point to newbie
				temps.set(level, temp);
				// go down a level
				level--;
				continue;
			}
			// moving forward wont skip so move forward
			temp = temp.next(level);
			continue;
		}
		// insert point found and interupted nodes noted
		
		int counter = 1;
		
		while ((rand.nextInt(2) == 1) && (counter < this.height))
		{
			counter++;
		}
		
		Node<N> newbie = new Node<N>(data, counter);
		
		// for the height of newbie, reassign pointers interupted by newbie
		for (int i = counter - 1; i >= 0; i--)
		{
			// set newbie's level pointer to what prev node was pointing to
			newbie.setNext(i, temps.get(i).next(i));
			// have prev node point to newbie
			temps.get(i).setNext(i, newbie);
		}
		
		this.size++;
		int log2n = SkipList.getMaxHeight(this.size);
		
		// check if size has grown, if yes, grow list
		if (log2n > this.height)
		{
			this.height = log2n;
			this.growSkipList();
		}
	}
	// same as insert with probalistic height removed for newbie
	public void insert(N data, int height)
	{
		if (this.height == 1 && this.size == 0)
		{
			Node<N> newbie = new Node<N>(data, 1);
			this.head.setNext(0, newbie); 
			this.size++;
			return;
		}
		else if (this.size == 0)
		{
			int counter = height;
		
			Node<N> newbie = new Node<N>(data, counter);
		
			for (int i = counter - 1; i >= 0; i--)
			{
				this.head.setNext(i, newbie);
			}
		
			this.size++;
			return;
		}
		
		Node<N> comp = new Node<N>(data, 0);
		ArrayList<Node<N>> temps = new ArrayList<Node<N>>(this.height);
		for (int x = 0; x < this.height; x++)
		{
			temps.add(null);
		}
		
		int level = this.height - 1;
		Node<N> temp = this.head;
		
		while (this.size > 0)
		{
			int comparison = 0;
			if (temp.next(level) != null)
			{
				comparison = temp.next(level).compareTo(comp);
			}
			if (temp.next(level) == null || comparison > 0 || comparison == 0)
			{
				if (level == 0)
				{
					temps.set(level, temp);
					break;
				}
				temps.set(level, temp);
				level--;
				continue;
			}
			temp = temp.next(level);
			continue;
		}
		int counter = height;
		
		Node<N> newbie = new Node<N>(data, counter);
		
		for (int i = counter - 1; i >= 0; i--)
		{
			newbie.setNext(i, temps.get(i).next(i));
			temps.get(i).setNext(i, newbie);
		}
		
		this.size++;
		int log2n = SkipList.getMaxHeight(this.size);
		
		if (log2n > this.height)
		{
			this.height = log2n;
			this.growSkipList();
		}
	}
	
	// removes node from skiplist
	public void delete(N data)
	{
		// list is empty
		if (this.size == 0)
		{
			return;
		}
		
		Node<N> comp = new Node<N>(data, 0);
		// keeps track of nodes to reassign after deletion
		ArrayList<Node<N>> temps = new ArrayList<Node<N>>(this.height);
		
		for (int x = 0; x < this.height; x++)
		{
			temps.add(null);
		}
		
		int level = this.height - 1;
		// counter now keeps track of how much of the temps array needs reassigning
		int counter = 0;
		Node<N> temp = this.head;
		// keeps track of the node to delete
		Node<N> delNode = null;
		
		while (this.size > 0)
		{
			// if current level points past node to delete
			if (temp.next(level) == null || temp.next(level).compareTo(comp) > 0)
			{
				if (level == 0)
				{
					// end of list, node not found
					return;
				}
				// go down a level
				level--;
				continue;
			}
			// node with value found
			if (temp.next(level).compareTo(comp) == 0)
			{
				// if node is duplicate node that comes before and is shorter
				if ((temp.next(level) != delNode) && (delNode != null))
				{
					// reassign node to delete
					delNode = temp.next(level);
					// restart counter for use of temps
					counter = 0;
				}
				else if (delNode == null)
				{
					// initial assignment of node to delete
					delNode = temp.next(level);
				}
				if (level == 0)
				{
					// tracked all nodes that need to be reassigned
					temps.set(level, temp);
					break;
				}
				// track node to reassign at current level
				temps.set(level, temp);
				level--;
				counter++;
				continue;
			}
			// moving forward wont skip value to delete
			temp = temp.next(level);
			continue;
		}
		
		// for each node pointing to delNode, reassign level pointer to delNode's
		for (int i = counter; i >= 0; i--)
		{
			temps.get(i).setNext(i, delNode.next(i));
		}
		
		// decrement size, leave height if <2
		this.size--;
		if (this.size == 1)
		{
			return;
		}
		else if (this.size == 0)
		{
			return;
		}
		// check if height changes, if yes, trim
		int log2n = SkipList.getMaxHeight(this.size);
		while (log2n < this.height)
		{
			this.trimSkipList();
			this.height--;
		}
	}
	// checks if skiplist contains data
	public boolean contains(N data)
	{
		if (this.size == 0)
		{
			return false;
		}
		
		Node<N> comp = new Node<N>(data, 0);
		int level = this.height - 1;
		Node<N> temp = this.head;
		
		while (this.size > 0)
		{
			if (temp.next(level) == null || temp.next(level).compareTo(comp) > 0)
			{
				if (level == 0)
				{
					
					return false;
				}
				level--;
				continue;
			}
			else if (temp.next(level).compareTo(comp) == 0)
			{
				return true;
			}
			temp = temp.next(level);
			continue;
		}
		return false;
	}
	
	// Same as contains() but returns node found
	public Node<N> get(N data)
	{
		if (this.size == 0)
		{
			return null;
		}
		
		Node<N> comp = new Node<N>(data, 0);
		int level = this.height - 1;
		Node<N> temp = this.head;
		while (this.size > 0)
		{
			if (temp.next(level) == null || temp.next(level).compareTo(comp) > 0)
			{
				if (level == 0)
				{
					
					return null;
				}
				level--;
				continue;
			}
			if (temp.next(level).compareTo(comp) == 0)
			{
				return temp.next(level);
			}
			temp = temp.next(level);
			continue;
		}
		return null;
	}
	
	// returns the cieling of the log base 2 of n
	private static int getMaxHeight(int n)
	{
		return (int)(Math.ceil((Math.log(n)/Math.log(2))));
	}
	
	// raises skiplist height by one
	private void growSkipList()
	{
		// grows head by one
		this.head.grow();
		Node<N> temp = this.head.next(this.height - 2);
		Node<N> temp2 = this.head;
		
		// 50% chance to grow each max height node by one to new height
		while (temp != null)
		{
			if (temp.maybeGrow())
			{
				temp2.setNext(this.height - 1, temp);
				temp2 = temp2.next(this.height - 1);
			}
			temp = temp.next(this.height - 2);
		}
	}
	
	// lowers the skiplist height by one
	private void trimSkipList()
	{
		Node<N> temp = this.head;
		int temh = this.head.height() - 1;
		Node<N> temp2 = this.head.next(temh);
		
		// trims each max height node by one
		while (temp2 != null)
		{
			temp.trim(temh);
			temp = temp2;
			temp2 = temp2.next(temh);
		}
		
		temp.trim(temh);
	}
	
	public static double difficultyRating()
	{
		return 4.0;
	}
	
	public static double hoursSpent()
	{
		return 20.0;
	}
}
