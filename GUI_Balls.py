import tkinter as tk
from tkinter import messagebox
import subprocess
import threading

BALL_RADIUS = 20
STACK_GAP = 80
COLOR_MAP = {
    1: "red",
    2: "green",
    3: "yellow",
    4: "purple",
    5: "orange",
    6: "pink"
}

class BallSortGUI:
    def __init__(self, master, initial_state):
        self.master = master
        self.state = [stack[:] for stack in initial_state]
        self.num_stacks = len(initial_state)
        self.moves_count = 0
        self.moves_queue = []

        self.canvas = tk.Canvas(master, width=self.num_stacks*STACK_GAP+50, height=300, bg="lightblue")
        self.canvas.pack(pady=10)

        self.label = tk.Label(master, text=f"Moves: {self.moves_count}", font=("Helvetica", 16, "bold"), bg="lightblue")
        self.label.pack()

        self.draw_stacks()

    def draw_stacks(self):
        self.canvas.delete("all")
        for i, stack in enumerate(self.state):
            x_center = 50 + i * STACK_GAP
            self.canvas.create_rectangle(x_center-25, 50, x_center+25, 250, outline="black", width=2)
            for j, ball in enumerate(stack):
                y_center = 250 - (j+1)*2*BALL_RADIUS + BALL_RADIUS
                self.canvas.create_oval(
                    x_center-BALL_RADIUS, y_center-BALL_RADIUS,
                    x_center+BALL_RADIUS, y_center+BALL_RADIUS,
                    fill=COLOR_MAP.get(ball, "gray"), outline="black"
                )
            self.canvas.create_text(x_center, 260, text=str(i), font=("Helvetica", 12, "bold"))

    def animate_next_move(self):
        if not self.moves_queue:
            return
        from_stack, to_stack = self.moves_queue.pop(0)
        if self.state[from_stack]:
            ball = self.state[from_stack].pop()
            self.state[to_stack].append(ball)
            self.moves_count += 1
            self.label.config(text=f"Moves: {self.moves_count}")
            self.draw_stacks()
        self.master.after(300, self.animate_next_move)

    def add_move(self, move):
        self.moves_queue.append(move)

    def start_animation(self):
        if self.moves_queue:
            self.master.after(500, self.animate_next_move)


class InputGUI:
    def __init__(self, master, cpp_path):
        self.master = master
        self.cpp_path = cpp_path
        master.title("Ball Sorting Game")
        master.configure(bg="lightblue")

        self.entries = {}
        self.labels = {}
        input_frame = tk.Frame(master, bg="lightblue")
        input_frame.pack(pady=20)

        custom_font = ("Helvetica", 14, "bold")

    
        for idx, label_text in enumerate(["Stacks", "Balls/Stack", "Colors", "Empty"]):
            lbl = tk.Label(input_frame, text=label_text, font=custom_font, bg="lightblue")
            lbl.grid(row=0, column=idx*2, padx=5)
            self.labels[label_text] = lbl

            ent = tk.Entry(input_frame, font=custom_font, width=5, justify="center")
            ent.grid(row=0, column=idx*2+1, padx=5)
            self.entries[label_text] = ent

        self.run_button = tk.Button(master, text="Run Solver", font=custom_font, bg="blue", fg="white",
                                    command=self.run_cpp_thread)
        self.run_button.pack(pady=10)

        self.status_label = tk.Label(master, text="", font=custom_font, bg="lightblue", fg="darkblue")
        self.status_label.pack(pady=5)

        self.anim_window = None
        self.gui = None

    def run_cpp_thread(self):
        for entry in self.entries.values():
            entry.config(state="disabled")
        self.run_button.config(state="disabled")

        threading.Thread(target=self.run_cpp_and_stream, daemon=True).start()

    def get_int(self, entry_name):
        value = self.entries[entry_name].get().strip()
        if not value:
            raise ValueError(f"{entry_name} cannot be empty")
        return int(value)

    def run_cpp_and_stream(self):
        try:
        
            try:
                stacks = self.get_int("Stacks")
                balls_per_stack = self.get_int("Balls/Stack")
                colors = self.get_int("Colors")
                empty = self.get_int("Empty")
            except ValueError as e:
                messagebox.showerror("Input Error", f"Please enter valid integers.\n{e}")
                return

            
            process = subprocess.Popen(
                [self.cpp_path],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )

           
            process.stdin.write(f"{stacks}\n{balls_per_stack}\n{colors}\n{empty}\n")
            process.stdin.flush()

           
            stdout_lines = process.stdout.read().splitlines()
            
            
            print("C++ output:")
            print("\n".join(stdout_lines))
            
            
            process.stdout.close()
            process.stdin.close()
            process.wait()

           
            initial_state = [[] for _ in range(stacks)]
            moves = []
            stack_counter = 0
            moves_section = False

            for line in stdout_lines:
                line = line.strip()
                if not line:
                    continue

                
                if line.upper().startswith("STACKS"):
                    continue
                if line.upper().startswith("MOVES"):
                    moves_section = True
                    continue

                nums = []
                for token in line.split():
                    if token.isdigit():
                        nums.append(int(token))
                if not nums:
                    continue

                if not moves_section:
                  
                    if stack_counter < stacks:
                        initial_state[stack_counter] = nums
                        stack_counter += 1
                else:
                 
                    if len(nums) == 2:
                        moves.append((nums[0], nums[1]))
                        print("Move added:", moves[-1])  

            if not moves:
                messagebox.showwarning("Warning", "No moves found. Check C++ output format.")
                return

          
            self.anim_window = tk.Toplevel(self.master)
            self.anim_window.title("Animation")
            self.anim_window.configure(bg="lightblue")

            self.gui = BallSortGUI(self.anim_window, initial_state)
            for move in moves:
                self.gui.add_move(move)

            self.status_label.config(text=f"Solver finished! Total moves: {len(moves)}")
            self.gui.start_animation()

        except Exception as e:
            messagebox.showerror("Error", f"Failed to run solver: {e}")

if __name__ == "__main__":
    cpp_exe_path = r"C:\Users\hanaa\Desktop\sems 5\advanced algorithms\coursework\task 8; ball sorting game\ballGame.exe"
    root = tk.Tk()
    input_gui = InputGUI(root, cpp_exe_path)
    root.mainloop()
