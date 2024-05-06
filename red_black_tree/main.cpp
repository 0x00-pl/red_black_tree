#include<iostream>
#include<list>
#include<vector>
using namespace std;
namespace pl_old{
template<typename T>
class red_black_tree{
public:
	static const unsigned int nil= 0;
	//辅助类_节点
	class node{
	public:
		struct side{
			static const int left=0;
			static const int right=1;
		};

		node(const T& _val){left()=right()=nil; color=red; val=_val;}
		~node(){delete l;  delete r;}
		
		node*& left(){return child[side::left];}
		node*& right(){return child[side::right];}
		int side_of(node* n){
			return n==child[side::left]? side::left: side::right;
		}
		node* other(node* n){
			return n==child[side::left]? child[side::right]: child[side::left];
		}
		static int other(int _side){return side::left==_side? side::right: side::left;}

	public:
		node* child[2];
		//还能这么写啊~真方便
		enum rb_color{red,black} color;
		//unsigned int size;
		T val;
	};
	//迭代器
	class iterator{
	public:
		iterator(){}
		iterator(const red_black_tree& t){ 
			reset(t);
		}
		void reset(const red_black_tree& t){
			path.clear();
			path.push_back(t.root);
		}
		void move_to_begin(){
			if(path.empty()) return;
			node* root= *path.begin();
			path.clear();
			path.push_back(root);
			while(move_left());
		}
		inline node* back_node(){return path.back();} 
		bool move_left(){
			if(path.empty()) return false;
			if(back_node()==nil || back_node()->left()== nil)
				return false;
			path.push_back(back_node()->left());
			return true;
		}
		bool move_right(){
			if(path.empty()) return false;
			if(back_node()==nil || back_node()->right()== nil)
				return false;
			path.push_back(back_node()->right());
			return true;
		}
		bool move_up(){
			if(path.size()<= 1) return false;
			path.pop_back();
			return true;
		}
		bool move_left_up_able(){
			if(path.size()<= 1) return false;
			list<node*>::iterator t= path.end();
			t--;
			t--;
			if( (*t)->right()!= back_node() ) return false;
			return true;
		}
		bool move_right_up_able(){
			if(path.size()<= 1) return false;
			list<node*>::iterator t= path.end();
			t--;
			t--;
			if( (*t)->left()!= path.back() ) 
				return false;
			return true;
		}

		bool move_next(){
			if(move_right()){
				while(move_left());
				return true;
			}else{
				while(!move_right_up_able())
					if(!move_up()) return false;
				move_up();
				return true;
			}
		}

		node* get(){return path.empty()? nil: path.back();}

		list<node*> path;
	};
	//构造函数
	red_black_tree(){root=nil;}
	//waring: copy the return;
	iterator& find_path(const T& val, iterator& out_path){
		out_path.reset(*this);

		while(out_path.back_node()!= nil){
			if(val< out_path.back_node()->val){
				if(!out_path.move_left())return out_path;
			}else{
				if(!out_path.move_right())return out_path;
			}
		}
		out_path.path.pop_back();
		return out_path;
	}
	iterator& find(const T& val, iterator& out_path){
		out_path.reset(*this);
		bool flag= true;
		while(flag){
			if(val== out_path.back_node()->val)
				return out_path;
			if(val< out_path.back_node()->val)
				flag=out_path.move_left();
			else
				flag=out_path.move_right();
		}
		out_path.path.clear();
		return out_path();
	}
	void insert_balance(iterator& path_i){
		//用while替代递归
		while(true){
			//level 0
			node* x= path_i.get();
			if(!path_i.move_up()){
				//这个是root
				x->color= node::black;
				return;
			}
			//level -1
			node* p1= path_i.get();
			if(!path_i.move_up()){
				//父节点是root
				p1->color= node::black;
				return;
			}
			//level -2
			node* p2= path_i.get();
			//镜像操作
			int main_side= p2->side_of(p1);
			int sub_side= p1->side_of(x);



			if(p1->color== node::black)//父节点黑色 END
				return;
			if(p2->color== node::red){//2级父节点红色 ERROR
				cout<<"红黑树崩坏: 父节点 和2级父节点 都是红色."<<endl;
				return;
			}
			if(p2->child[node::other(main_side)]!= nil &&
				p2->child[node::other(main_side)]->color== node::red){
				//```Bp2``
				//`Rp1``R`
				//`Rx`````
				p2->color= node::red;
				p2->child[node::other(main_side)]->color= node::black;
				p1->color= node::black;
				//```Rp2``
				//`Bp1``B`
				//`Rx`````
				continue;//递归 insert_balance()
			}
			//```Bp2``
			//`Rp1``B`
			//`Rx`````
			node *a, *b, *c, *d;
			if(main_side!= sub_side){
				//````Bp2`
				//`Rp1```d
				//a``Rx```
				//``b`c```
				a= p1->child[main_side];
				b= x->child[main_side];
				c= x->child[node::other(main_side)];
				d= p2->child[node::other(main_side)];

				p1->child[node::other(main_side)]= b;
				x->child[main_side]= p1;
				p2->child[main_side]= x;
				//`````Bp2```
				//``Rx````Bd`
				//`Rp1`c`````
				//a``b````````
				swap(x, p1);
			}
			//`````Bp2```
			//``Rp1```Bd`
			//`Rx`c``````
			//a`b````````
			a= x->child[main_side];
			b= x->child[node::other(main_side)];
			c= p1->child[node::other(main_side)];
			d= p2->child[node::other(main_side)];
			
			p1->child[node::other(main_side)]= p2;
			p2->child[main_side]= c;
			p1->color= node::black;
			p2->color= node::red;
			//````Bp1````
			//``Rx```Rp2`
			//`a``b``c``d
			if(root== p2){
				root= p1;
			}else{
				if(!path_i.move_up())//ERROR
					return;
				node* p3=path_i.get();
				p3->child[p3->side_of(p2)]= p1;
			}
			return;
		}
	}
	void remove_balance(){}
	void insert(const T& val){
		//添加内存池修改此代码...
		node* new_node= new node(val);

		iterator dest;
		find_path(val, dest);
		if(dest.path.size()< 1){
			root= new_node;
			root->color= node::black;
			return;
		}

		if(val< dest.back_node()->val)
			dest.back_node()->left()= new_node;
		else
			dest.back_node()->right()= new_node;

		dest.path.push_back(new_node);

		insert_balance(dest);
	}
	void remove(const T& val){
		iterator path;
	}
	iterator begin(){
		iterator r(*this);
		r.move_to_begin();
		return r;
	}


	node* root;
};
}

namespace pl{
	typedef int _Tval;
	const bool auto_del= false;
	struct simple_rb_node{
		typedef simple_rb_node node_type;
		struct link_inedx{
			static const int parent=0;
			static const int left=1;
			static const int right=2;
			static const int size=3;
		};
		/*struct iterator{
			iterator(node_type* _ptr=0){}
			_Tval* get_val(){
				return node? node->val: 0;
			}
			iterator left(){
				//TODO
				return *this;
			}
			iterator right(){
				//TODO
				return *this;
			}
			iterator parent(){
				//TODO
				return *this;
			}
			bool color(){
				return node? node->is_red: false;
			}
			void update(){
				node->size= val? 1: 0+
					node->left()->size+
					node->left()->size;

			}
			node_type* get_node(){return node;}
			node_type* node;
		};*/

		simple_rb_node(){
			size= 0;
			val= 0;
			links[link_inedx::parent]= 0;
			links[link_inedx::left]= 0;
			links[link_inedx::right]= 0;
			is_red= true;
		}
		node_type*& left(){
			return links[link_inedx::left];
		}
		node_type*& right(){
			return links[link_inedx::right];
		}
		node_type*& parent(){
			return links[link_inedx::parent];
		}
		bool& color(){
			return is_red;
		}
		void update(){
			size=  1+
				left()->size+
				left()->size;
		}

		unsigned int size;
		node_type* links[link_inedx::size];
		bool is_red;
		_Tval* val;
	};


	typedef simple_rb_node _Tnode;
	_Tnode*& (_Tnode::*get_left)()= &_Tnode::left;
	_Tnode*& (_Tnode::*get_right)()= &_Tnode::right;
	_Tnode*& (_Tnode::*get_parent)()= &_Tnode::parent;
	bool& (_Tnode::*get_color)()= &_Tnode::color;
	void (_Tnode::*update)()= &_Tnode::update;
	struct rb_tree{
		rb_tree():root(&_end){}

		bool insert(){return false;}
		bool erase(){return false;}
	private:
		void _rote(_Tnode*& sub_root, bool from_left){
			_Tnode* sub_node;
			if(from_left)
			{
				sub_node= (sub_root->*get_left)();
				(sub_root->*get_left)()= (sub_node->*get_right)();
				(sub_node->*get_right)()= sub_root;
			}else{
				sub_node= (sub_root->*get_right)();
				(sub_root->*get_right)()= (sub_node->*get_left)();
				(sub_node->*get_left)()= sub_root;
			}

			(sub_node->*get_parent)()= (sub_root->*get_parent)();
			(sub_root->*get_parent)()= sub_node;

			swap( (sub_root->*get_color)(), (sub_node->*get_color)());

			(sub_root->*update)();
			(sub_node->*update)();

			sub_root= sub_node;
		}
		void insert_balance(_Tnode* inserted){
			while( (inserted->*get_color)()== true ){
				_Tnode* parent= (inserted->*get_parent)();

				if( parent== 0 ){
					(inserted->*get_color)()= false;
					break;
				}
				if( (parent->*get_color)()== false ) break;

				_Tnode* parent2= (parent->*get_parent)();


				//case 3:
				if( ((parent2->*get_left)()->*get_color)()==true &&
					((parent2->*get_right)()->*get_color)()==true )
				{
					((parent2->*get_left)()->*get_color)()=
						((parent2->*get_right)()->*get_color)()=
						false;
					(parent2->*get_color)()= true;
					inserted= parent2;
					continue;
				}
				//case 1: case 2:
				if( (parent2->*get_left)()== parent ){
					//left
					if( (parent->*get_right)()== inserted ){
						//case 2:
						_rote( (parent2->*get_left)(), false);
					}
					//case 1:
					if( (parent2->*get_parent)()!= 0 ){
						_Tnode* parent3=(parent2->*get_parent)();
						_Tnode*& temp_pnode= (parent3->*get_left)()==parent2? (parent3->*get_left)(): (parent3->*get_right)();
						_rote(temp_pnode, true);
					}else{
						_rote(root, true);
					}
				}else{
					//right
					if( (parent->*get_left)()== inserted ){
						//case 2:
						_rote( (parent2->*get_right)(), true);
					}
					//case 1:
					if( (parent2->*get_parent)()!= 0 ){
						_Tnode* parent3=(parent2->*get_parent)();
						_Tnode*& temp_pnode= (parent3->*get_left)()==parent2? (parent3->*get_left)(): (parent3->*get_right)();
						_rote(temp_pnode, false);
					}else{
						_rote(root, false);
					}
				}
				break;
			}
		}
	public:
		_Tnode _end;
		_Tnode* root;
	};
}









void main() {
	struct{int:2;} t,*pt,t2[4];
	int tempaaaa;
	cin>>tempaaaa;
}