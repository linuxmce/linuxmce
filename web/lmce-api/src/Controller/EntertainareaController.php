<?php
namespace App\Controller;

use App\Controller\AppController;

/**
 * Entertainarea Controller
 *
 * @property \App\Model\Table\EntertainareaTable $Entertainarea
 *
 * @method \App\Model\Entity\Entertainarea[]|\Cake\Datasource\ResultSetInterface paginate($object = null, array $settings = [])
 */
class EntertainareaController extends AppController
{

    /**
     * Index method
     *
     * @return \Cake\Http\Response|void
     */
    public function index()
    {
        $this->paginate = [
            'contain' => ['Room']
        ];
        $entertainarea = $this->paginate($this->Entertainarea);

        $this->set(compact('entertainarea'));
    }

    /**
     * View method
     *
     * @param string|null $id Entertainarea id.
     * @return \Cake\Http\Response|void
     * @throws \Cake\Datasource\Exception\RecordNotFoundException When record not found.
     */
    public function view($id = null)
    {
        $entertainarea = $this->Entertainarea->get($id, [
            'contain' => ['Room', 'CommandGroup']
        ]);

        $this->set('entertainarea', $entertainarea);
    }

    /**
     * Add method
     *
     * @return \Cake\Http\Response|null Redirects on successful add, renders view otherwise.
     */
    public function add()
    {
        $entertainarea = $this->Entertainarea->newEntity();
        if ($this->request->is('post')) {
            $entertainarea = $this->Entertainarea->patchEntity($entertainarea, $this->request->getData());
            if ($this->Entertainarea->save($entertainarea)) {
                $this->Flash->success(__('The entertainarea has been saved.'));

                return $this->redirect(['action' => 'index']);
            }
            $this->Flash->error(__('The entertainarea could not be saved. Please, try again.'));
        }
        $room = $this->Entertainarea->Room->find('list', ['limit' => 200]);
        $commandGroup = $this->Entertainarea->CommandGroup->find('list', ['limit' => 200]);
        $this->set(compact('entertainarea', 'room', 'commandGroup'));
    }

    /**
     * Edit method
     *
     * @param string|null $id Entertainarea id.
     * @return \Cake\Http\Response|null Redirects on successful edit, renders view otherwise.
     * @throws \Cake\Network\Exception\NotFoundException When record not found.
     */
    public function edit($id = null)
    {
        $entertainarea = $this->Entertainarea->get($id, [
            'contain' => ['CommandGroup']
        ]);
        if ($this->request->is(['patch', 'post', 'put'])) {
            $entertainarea = $this->Entertainarea->patchEntity($entertainarea, $this->request->getData());
            if ($this->Entertainarea->save($entertainarea)) {
                $this->Flash->success(__('The entertainarea has been saved.'));

                return $this->redirect(['action' => 'index']);
            }
            $this->Flash->error(__('The entertainarea could not be saved. Please, try again.'));
        }
        $room = $this->Entertainarea->Room->find('list', ['limit' => 200]);
        $commandGroup = $this->Entertainarea->CommandGroup->find('list', ['limit' => 200]);
        $this->set(compact('entertainarea', 'room', 'commandGroup'));
    }

    /**
     * Delete method
     *
     * @param string|null $id Entertainarea id.
     * @return \Cake\Http\Response|null Redirects to index.
     * @throws \Cake\Datasource\Exception\RecordNotFoundException When record not found.
     */
    public function delete($id = null)
    {
        $this->request->allowMethod(['post', 'delete']);
        $entertainarea = $this->Entertainarea->get($id);
        if ($this->Entertainarea->delete($entertainarea)) {
            $this->Flash->success(__('The entertainarea has been deleted.'));
        } else {
            $this->Flash->error(__('The entertainarea could not be deleted. Please, try again.'));
        }

        return $this->redirect(['action' => 'index']);
    }
}
