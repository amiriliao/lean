set_option new_elaborator true

section
  variable (A : Type)

  definition f : A → A :=
  λ x, x

end

check f
