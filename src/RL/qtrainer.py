import tensorflow as tf
import rlenv
import numpy as np
import uw_random
import pyosr

class QTrainer:
    def __init__(self,
            advcore,
            batch,
            learning_rate,
            ckpt_dir,
            period,
            global_step
            ):
        self.advcore = advcore
        self.batch = batch
        self.period = period
        self.optimizer = tf.train.AdamOptimizer(learning_rate=learning_rate)
        self.loss = self.build_loss(advcore)
        self.global_step = global_step
        tf.summary.scalar('q_loss', self.loss)
        if ckpt_dir is not None:
            self.summary_op = tf.summary.merge_all()
            self.train_writer = tf.summary.FileWriter(ckpt_dir + '/summary', tf.get_default_graph())
        self.train_op = self.optimizer.minimize(self.loss, global_step=global_step, var_list=advcore.valparams)

    def build_loss(self, advcore):
        self.V_tensor = tf.placeholder(tf.float32, shape=[None], name='VPh')
        flattened_value = tf.reshape(advcore.value, [-1])
        return tf.nn.l2_loss(self.V_tensor - flattened_value)

    UP = np.array([0,0,1])

    def calculate_q(self, envir, states):
        r = envir.r
        p = r.perturbation
        rot = pyosr.extract_rotation_matrix(p)
        up = rot.dot(self.UP)
        origin = p[0:3]
        # print(up)
        # print(states[0][0:3] - origin)
        return [abs(np.dot(up, state[0:3] - origin)) for state in states]

    def render(self, envir, state):
        envir.qstate = state
        return envir.vstate

    def train(self, envir, sess, tid=None):
        states = [uw_random.gen_unit_init_state(envir.r) for i in range(self.batch)]
        values = self.calculate_q(envir, states)
        values = np.array(values, dtype=np.float)
        images = [self.render(envir, state) for state in states]
        batch_rgb = [image[0] for image in images]
        batch_dep = [image[1] for image in images]
        advcore = self.advcore
        dic = {
                advcore.rgb_1: batch_rgb,
                advcore.dep_1: batch_dep,
                self.V_tensor: values,
              }
        # print(values)
        # print(values.shape)
        loss, _, summary, step = sess.run([self.loss, self.train_op, self.summary_op, self.global_step], feed_dict=dic)
        self.train_writer.add_summary(summary, step)
        print("Current loss {}".format(loss))
        if envir.steps_since_reset >= self.period * self.batch:
            envir.reset()