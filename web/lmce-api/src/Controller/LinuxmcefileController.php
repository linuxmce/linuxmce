<?php
namespace App\Controller;

use App\Controller\AppController;

/**
 * Linuxmcefile Controller
 *
 * @property \App\Model\Table\LinuxmcefileTable $Linuxmcefile
 *
 * @method \App\Model\Entity\Linuxmcefile[]|\Cake\Datasource\ResultSetInterface paginate($object = null, array $settings = [])
 */
class LinuxmcefileController extends AppController
{
	
	 public function initialize()
    {
        parent::initialize();
		 $this->loadComponent('RequestHandler');
		$this->loadComponent('DceCommandExecutor');
    }

    /**
     * Index method
     *
     * @return \Cake\Http\Response|void
     */
    public function index()
    {
        $linuxmcefile = $this->paginate($this->Linuxmcefile);

        $this->set(compact('linuxmcefile'));
    }

    /**
     * View method
     *
     * @param string|null $id Linuxmcefile id.
     * @return \Cake\Http\Response|void
     * @throws \Cake\Datasource\Exception\RecordNotFoundException When record not found.
     */
    public function view($id = null)
    {
        $linuxmcefile = $this->Linuxmcefile->get($id, [
            'contain' => []
        ]);

        $this->set('linuxmcefile', $linuxmcefile);
    }

    /**
     * Add method
     *
     * @return \Cake\Http\Response|null Redirects on successful add, renders view otherwise.
     */
    public function add()
    {
        $linuxmcefile = $this->Linuxmcefile->newEntity();
        if ($this->request->is('post')) {
            $linuxmcefile = $this->Linuxmcefile->patchEntity($linuxmcefile, $this->request->getData());
            if ($this->Linuxmcefile->save($linuxmcefile)) {
                $this->Flash->success(__('The linuxmcefile has been saved.'));

                return $this->redirect(['action' => 'index']);
            }
            $this->Flash->error(__('The linuxmcefile could not be saved. Please, try again.'));
        }
        $this->set(compact('linuxmcefile'));
    }

    /**
     * Edit method
     *
     * @param string|null $id Linuxmcefile id.
     * @return \Cake\Http\Response|null Redirects on successful edit, renders view otherwise.
     * @throws \Cake\Network\Exception\NotFoundException When record not found.
     */
    public function edit($id = null)
    {
        $linuxmcefile = $this->Linuxmcefile->get($id, [
            'contain' => []
        ]);
        if ($this->request->is(['patch', 'post', 'put'])) {
            $linuxmcefile = $this->Linuxmcefile->patchEntity($linuxmcefile, $this->request->getData());
            if ($this->Linuxmcefile->save($linuxmcefile)) {
                $this->Flash->success(__('The linuxmcefile has been saved.'));

                return $this->redirect(['action' => 'index']);
            }
            $this->Flash->error(__('The linuxmcefile could not be saved. Please, try again.'));
        }
        $this->set(compact('linuxmcefile'));
    }

    /**
     * Delete method
     *
     * @param string|null $id Linuxmcefile id.
     * @return \Cake\Http\Response|null Redirects to index.
     * @throws \Cake\Datasource\Exception\RecordNotFoundException When record not found.
     */
    public function delete($id = null)
    {
        $this->request->allowMethod(['post', 'delete']);
        $linuxmcefile = $this->Linuxmcefile->get($id);
        if ($this->Linuxmcefile->delete($linuxmcefile)) {
            $this->Flash->success(__('The linuxmcefile has been deleted.'));
        } else {
            $this->Flash->error(__('The linuxmcefile could not be deleted. Please, try again.'));
        }

        return $this->redirect(['action' => 'index']);
    }
	
	public function findVideoFile(){
		$name = $this->request->params['?']['name'];
		
		$movieReply = $this->Linuxmcefile->getVideoByName($name);
			
		$final = array();
		$final['movieAnswer'] = $movieReply;
		
		$this->set(compact('final'));
		$this->set('_serialize', 'final');
	}
	
	
}
