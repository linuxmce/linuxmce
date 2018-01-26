<?php
namespace App\Controller;

use App\Controller\AppController;

/**
 * Commandgroup Controller
 *
 * @property \App\Model\Table\CommandgroupTable $Commandgroup
 *
 * @method \App\Model\Entity\Commandgroup[]|\Cake\Datasource\ResultSetInterface paginate($object = null, array $settings = [])
 */
class CommandgroupController extends AppController
{

    /**
     * Index method
     *
     * @return \Cake\Http\Response|void
     */
    public function index()
    {
        $commandgroup = $this->paginate($this->Commandgroup);

        $this->set(compact('commandgroup'));
    }

    /**
     * View method
     *
     * @param string|null $id Commandgroup id.
     * @return \Cake\Http\Response|void
     * @throws \Cake\Datasource\Exception\RecordNotFoundException When record not found.
     */
    public function view($id = null)
    {
        $commandgroup = $this->Commandgroup->get($id, [
            'contain' => ['Room']
        ]);

        $this->set('commandgroup', $commandgroup);
    }

    /**
     * Add method
     *
     * @return \Cake\Http\Response|null Redirects on successful add, renders view otherwise.
     */
    public function add()
    {
        $commandgroup = $this->Commandgroup->newEntity();
        if ($this->request->is('post')) {
            $commandgroup = $this->Commandgroup->patchEntity($commandgroup, $this->request->getData());
            if ($this->Commandgroup->save($commandgroup)) {
                $this->Flash->success(__('The commandgroup has been saved.'));

                return $this->redirect(['action' => 'index']);
            }
            $this->Flash->error(__('The commandgroup could not be saved. Please, try again.'));
        }
        $this->set(compact('commandgroup'));
    }

    /**
     * Edit method
     *
     * @param string|null $id Commandgroup id.
     * @return \Cake\Http\Response|null Redirects on successful edit, renders view otherwise.
     * @throws \Cake\Network\Exception\NotFoundException When record not found.
     */
    public function edit($id = null)
    {
        $commandgroup = $this->Commandgroup->get($id, [
            'contain' => []
        ]);
        if ($this->request->is(['patch', 'post', 'put'])) {
            $commandgroup = $this->Commandgroup->patchEntity($commandgroup, $this->request->getData());
            if ($this->Commandgroup->save($commandgroup)) {
                $this->Flash->success(__('The commandgroup has been saved.'));

                return $this->redirect(['action' => 'index']);
            }
            $this->Flash->error(__('The commandgroup could not be saved. Please, try again.'));
        }
        $this->set(compact('commandgroup'));
    }

    /**
     * Delete method
     *
     * @param string|null $id Commandgroup id.
     * @return \Cake\Http\Response|null Redirects to index.
     * @throws \Cake\Datasource\Exception\RecordNotFoundException When record not found.
     */
    public function delete($id = null)
    {
        $this->request->allowMethod(['post', 'delete']);
        $commandgroup = $this->Commandgroup->get($id);
        if ($this->Commandgroup->delete($commandgroup)) {
            $this->Flash->success(__('The commandgroup has been deleted.'));
        } else {
            $this->Flash->error(__('The commandgroup could not be deleted. Please, try again.'));
        }

        return $this->redirect(['action' => 'index']);
    }
}
